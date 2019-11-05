// #include <iostream>
// #include <fcntl.h>
#include "CLTable.h"
using namespace std;
// #define BUF_MAXSIZE 3000  //100位int64_t数据存入字符串最长长度

unsigned long CLTable::row_nums = 0;
CLTable* CLTable::_pTable = 0;
pthread_mutex_t* CLTable::_pMutexForCreatingTable = CLTable::InitializeMutex();

//初始化创建表的信号量
pthread_mutex_t* CLTable::InitializeMutex() {
    pthread_mutex_t* p = new pthread_mutex_t;
    if(pthread_mutex_init(p, 0) != 0) {//初始化失败
        delete p;
        return 0;
    }

    return p;
}

//默认初始化构造
CLTable::CLTable() {
    _name = "table";
    _filename = "table";
    for(int i = 0; i < COLUMN_NUMS; i++) _atts_name[i] = to_string(i);
    // _rows = NULL;
    //打开文件，若不存在则创建文件
    _fd = open(_filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
    if(_fd == -1) throw "在CLTable::CLTable()初始化时, 打开文件失败";
    //实例指针和退出标志
    _pTable = this;
    _bFlagProcessExit = false;
    //初始化信号量
    _pMutexForInsert = new pthread_mutex_t;
    if(pthread_mutex_init(_pMutexForInsert, 0) != 0) {
        delete _pMutexForInsert;
        close(_fd);         
        throw "在CLTable::CLTable()初始化时，信号量初始化失败";
    }

    //索引初始化
    //读也需要加锁，因为插入的时候也会插入索引
    if(pthread_mutex_lock(_pMutexForInsert) != 0) throw "在初始化加载索引时加锁失败";
    try
    {
        _index = new SIndex(_atts_name[0], 0, _name + "Index");
        CLStatus s = _index->ReadIndex();
        if(!s.IsSuccess()) throw s;

        row_nums = _index->_index_count;

        throw CLStatus(0, 0);
    }
    catch(CLStatus &s)
    {
        //防止出错时不能解锁，必须先解锁，再处理错误
        if(pthread_mutex_unlock(_pMutexForInsert) != 0) throw "在初始化加载索引时解锁失败";

        if(!s.IsSuccess()) throw "索引读取失败";
    }

    // CLTable(_name, _filename, _atts_name, _atts_name[0]);
}


//指定name的构造函数
CLTable::CLTable(string name, string filename, string atts_name[], string index_att) {
    _name = name;
    _filename = filename;
    for(int i = 0; i < COLUMN_NUMS; i++) _atts_name[i] = atts_name[i];
    //打开文件，若不存在则创建文件
    _fd = open(_filename.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
    if(_fd == -1) throw "在CLTable::CLTable()初始化时, 打开文件失败";
    //实例指针和退出标志
    _pTable = this;
    _bFlagProcessExit = false;
    //初始化信号量
    _pMutexForInsert = new pthread_mutex_t;
    if(pthread_mutex_init(_pMutexForInsert, 0) != 0) {
        delete _pMutexForInsert;
        close(_fd);         
        throw "在CLTable::CLTable()初始化时，信号量初始化失败";
    }

    //索引初始化
    //读也需要加锁，因为插入的时候也会插入索引
    if(pthread_mutex_lock(_pMutexForInsert) != 0) throw "在初始化加载索引时加锁失败";
    try
    {
        int index_att_index = 0;      //索引属性下标默认为0
        for(int i = 0; i < COLUMN_NUMS; i++) if(_atts_name[i] == index_att) index_att_index = i;
        _index = new SIndex(index_att, index_att_index, _filename + "Index");
        CLStatus s = _index->ReadIndex();
        if(!s.IsSuccess()) throw s;

        row_nums = _index->_index_count;

        throw CLStatus(0, 0);
    }
    catch(CLStatus& s)
    {
        if(pthread_mutex_unlock(_pMutexForInsert) != 0) throw "在初始化加载索引时解锁失败";

        if(!s.IsSuccess()) throw "索引读取失败";
    }
    
}

//可以直接通过类调用，不通过对象，在表格的末尾添加一行数据
CLStatus CLTable::InsertDataLast(SRow *row) {
    CLTable *pTable = CLTable::GetInstance();
    if(pTable == 0) return CLStatus(-1, 0);

    CLStatus s = pTable->InsertLast(row);
    if(s.IsSuccess()) return CLStatus(0, 0);
    else return CLStatus(-1, 0);
}

//在表格的末尾添加一行数据
CLStatus CLTable::InsertLast(SRow *row) {
    if(row_nums == MAX_ROWS) {
        cout << "已达到最大行数，无法添加数据" << endl;
        return CLStatus(-1, 0); 
    }
    //若为null，可以手动输入一行数据
    if(!row) {
        row = new SRow();
        for(int i = 0; i < COLUMN_NUMS; i++) {
            cout << _atts_name[i] << ": ";
            cin >> row->atts[i];
            cout << endl;
        }
    }

    //写入元组，首先加锁，然后要先把文件偏移量转到最后
    //尝试加锁，若失败则返回
    if(pthread_mutex_lock(_pMutexForInsert) != 0) return CLStatus(-1, 0);
    try
    {
        if(_fd == -1) throw CLStatus(-1, errno);
        //移动文件偏移量至最后
        off_t currentPosition = lseek(_fd, 0, SEEK_END);    
        ssize_t writeBytes = write(_fd, row, sizeof(SRow));
        if(writeBytes == -1) throw CLStatus(-1, errno);
        row_nums++;

        //同时写入索引
        _index->_index[_index->_index_count].offset = currentPosition;
        _index->_index[_index->_index_count].value = row->atts[_index->_att_index];
        CLStatus s = _index->WriteIndex();
        if(!s.IsSuccess()) {
            cout << "索引写入失败" << endl;
            throw s;
        }
        else cout << "索引写入成功" << endl;

        throw CLStatus(0, 0);
    }
    catch(CLStatus& s) 
    {
        //解锁失败，返回
        if(pthread_mutex_unlock(_pMutexForInsert) != 0) return CLStatus(-1, 0);

        return s;
    }
}

//可以直接通过类调用，不通过对象，查询符合条件的元组
CLStatus CLTable::SelectData(string att_name, int64_t lo, int64_t hi) {
    CLTable *pTable = CLTable::GetInstance();
    if(pTable == 0) return CLStatus(-1, 0);

    CLStatus s = pTable->Select(att_name, lo, hi);
    if(s.IsSuccess()) return CLStatus(0, 0);
    else return CLStatus(-1, 0);
}

//查询符合条件的元组（最大数量显示为10），直接输出
CLStatus CLTable::Select(string att_name, int64_t lo, int64_t hi) {
    SRow row;                       //存储临时元组
    int count = 0;                  //符合条件的元组数
    ssize_t readBytes = 0;          //每次读到的字节数
    int att_num = -1;               //属性的列
    for(int i = 0; i < COLUMN_NUMS; i++) if(_atts_name[i] == att_name) att_num = i;
    if(_fd == -1) return CLStatus(-1, errno);
    //如果有索引，则直接在索引中查询
    //首先加锁，因为其他线程在写也会影响到此处的读
    if(pthread_mutex_lock(_pMutexForInsert) != 0) return CLStatus(-1, 0);
    try
    {
        if(_index->_att == att_name) {
            cout << "使用索引表查询属性\"" << att_name << "\"大于" << lo << "小于等于" << hi << "的元组……" << endl;
            for(int i = 0; i < _index->_index_count; i++){ 
                if(_index->_index[i].value > lo && _index->_index[i].value <= hi) {
                    lseek(_fd, _index->_index[i].offset, SEEK_SET);
                    ssize_t readBytes = read(_fd, &row, sizeof(row));
                    if(readBytes == -1) return CLStatus(-1, errno);
                    cout << row << endl;
                    count++;
                }
            }
        }else {
                //检索元组，但首先要把文件偏移量转到开头
            lseek(_fd, 0, SEEK_SET);
            cout << "正在查询属性\"" << att_name << "\"大于" << lo << "小于等于" << hi << "的元组……" << endl;
            while(count < 10) {
                readBytes = read(_fd, &row, sizeof(row));
                if(readBytes == -1) throw CLStatus(-1, errno);
                else if(readBytes == 0) break;
                if(row.atts[att_num] > lo && row.atts[att_num] <= hi)
                    cout << row << endl;
                count++;
            }
        }
    
        throw CLStatus(0, 0);   
    }
    catch(CLStatus& s)
    {
        //先解锁，防止有异常解不了锁妨碍其他线程
        if(pthread_mutex_unlock(_pMutexForInsert) != 0) return CLStatus(-1, 0);

        return s;
    }
}

//查询符合条件的元组（最大数量为10？这并不必要），返回元组数组的指针
SRow* CLTable::SelectReturn(string att_name, int64_t lo, int64_t hi) {
    SRow tmp[10];                   //最多返回十行元组
    int count = 0;                  //符合条件的元组数
    SRow row;                       //存储临时元组
    ssize_t readBytes = 0;          //每次读到的字节数
    int att_num = -1;               //属性的列
    for(int i = 0; i < COLUMN_NUMS; i++) if(_atts_name[i] == att_name) att_num = i;
    if(_fd == -1) {
        cout << "打开错误" << endl;
        cout << "错误编号是" << errno << endl;
        cout << strerror(errno) << endl;
        return NULL;
    }
    //检索元组，但首先要把文件偏移量转到开头
    lseek(_fd, 0, SEEK_SET);
    while(count < 10) {
        // char buf[BUF_MAXSIZE];
        readBytes = read(_fd, (void *)&row, sizeof(row));
        // cout << buf << endl;
        // cout << "读到字节数：" << readBytes << endl;
        if(readBytes == -1) {
            cout << "写入错误" << endl;
            cout << "错误编号是" << errno << endl;
            cout << strerror(errno) << endl;
            break;
        }else if(readBytes == 0) break;
        //判断该行是否满足条件，满足则加进去
        // cout << "读到元组：" << row << endl;
        if(row.atts[att_num] > lo && row.atts[att_num] <= hi)
            tmp[count++] = row;
    }    
    // cout << "匹配的行数为" << count << endl;

    //返回符合条件的元组，此处可优化
    SRow* res = NULL;
    if(count > 0) {
        res = new SRow[count]();
        for(int i = 0; i < count; i++) res[i] = tmp[i];
    }
    return res;
}


CLTable::~CLTable() {
    // if(_rows) delete _rows;
    if(_fd != -1) close(_fd);
}


//多线程情况下创建表需要加锁
CLTable* CLTable::GetInstance() {
    //双检测机制，若有直接返回，免去下面不必要的加锁解锁
    if(_pTable != 0) return _pTable;

    if(_pMutexForCreatingTable == 0) return 0;
    if(pthread_mutex_lock(_pMutexForCreatingTable) != 0) return 0;
    if(_pTable == 0) {
        try
        {
            _pTable = new CLTable();
        }
        catch(const char*)  //在初始化的时候throw除了字符串形式的异常
        {
            pthread_mutex_unlock(_pMutexForCreatingTable);
            return 0;
        }

        if(atexit(CLTable::OnProcessExit) != 0) {
            _pTable->_bFlagProcessExit = true;
            if(pthread_mutex_unlock(_pMutexForCreatingTable) != 0) return 0;
            cout << "在CLTable::GetInstance()中，atexit错误" << endl;
        }else if(pthread_mutex_unlock(_pMutexForCreatingTable) != 0) return 0;

        return _pTable;
    }

    if(pthread_mutex_unlock(_pMutexForCreatingTable) != 0) return 0;
    
    return _pTable;
}


//处理线程退出
void CLTable::OnProcessExit() {
    CLTable* pTable = CLTable::GetInstance();
    if(pTable != 0) {
        pthread_mutex_lock(pTable->_pMutexForInsert);
        pTable->_bFlagProcessExit = true;
        pthread_mutex_unlock(pTable->_pMutexForInsert);

        //在这里可以加入缓存机制
    }
}

CLStatus SIndex::WriteIndex() {
    if(_index_count == MAX_ROWS) {
        cout << "已达到最大行数，无法添加数据" << endl;
        return CLStatus(-1, 0);
    }

    if(_fd == -1) return CLStatus(-1, errno);
    //文件偏移量移到末尾
    lseek(_fd, 0, SEEK_END);    
    ssize_t writeBytes = write(_fd, &_index[_index_count], sizeof(SIndexPair));
    if(writeBytes == -1) 
        return CLStatus(-1, errno);

    _index_count++;
    return CLStatus(0, 0);
}

CLStatus SIndex::ReadIndex() {
    if(_fd == -1) return CLStatus(-1, errno);
    //文件偏移量移到开头
    lseek(_fd, 0, SEEK_SET);
    ssize_t readBytes = read(_fd, _index, MAX_ROWS * sizeof(SIndexPair));
    if(readBytes == -1) return CLStatus(-1, errno);
    _index_count = readBytes / sizeof(SIndexPair);
    cout << "初始化时索引数为" << _index_count << endl;

    return CLStatus(0, 0);
}