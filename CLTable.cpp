// #include <iostream>
// #include <fcntl.h>
#include "CLTable.h"
using namespace std;
#define BUF_MAXSIZE 3000  //100位int64_t数据存入字符串最长长度

CLTable* CLTable::_pTable = 0;

//默认初始化构造
CLTable::CLTable() {
    _name = "table";
    _filename = "table";
    for(int i = 0; i < COLUMN_NUMS; i++) _atts_name[i] = to_string(i);
    // _rows = NULL;
    _pTable = this;

    //打开文件，若不存在则创建文件
    _fd = open(_filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
}

//指定name的构造函数
CLTable::CLTable(string name, string filename, string atts_name[]) {
    _name = name;
    _filename = filename;
    for(int i = 0; i < COLUMN_NUMS; i++) _atts_name[i] = atts_name[i];
    // _rows = NULL;
    _pTable = this;

    //打开文件，若不存在则创建文件
    _fd = open(_filename.c_str(), O_RDWR | O_CREAT | O_APPEND, S_IRUSR | S_IWUSR);
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
    //若为null，可以手动输入一行数据
    if(!row) {
        row = new SRow();
        for(int i = 0; i < COLUMN_NUMS; i++) {
            cout << _atts_name[i] << ": ";
            cin >> row->atts[i];
            cout << endl;
        }
    }
    
    //对元组进行处理，将其转换为字符串
    // string buf;
    // for(int i = 0; i < COLUMN_NUMS; i++) {
    //     buf += to_string(row->atts[i]) + " ";
    // }
    // buf += "\r\n";

    //写入元组，首先把文件偏移量转到最后
    lseek(_fd, 0, SEEK_END);
    if(_fd == -1) 
        return CLStatus(-1, errno);
    ssize_t writeBytes = write(_fd, row, sizeof(SRow));
    if(writeBytes == -1) {
        return CLStatus(-1, errno);
    }

    return CLStatus(0, 0);
}

//
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
    //检索元组，但首先要把文件偏移量转到开头
    lseek(_fd, 0, SEEK_SET);
    cout << "正在查询属性\"" << att_name << "\"大于" << lo << "小于等于" << hi << "的元组……" << endl;
    while(count < 10) {
        readBytes = read(_fd, &row, sizeof(row));
        if(readBytes == -1) return CLStatus(-1, errno);
        else if(readBytes == 0) break;
        if(row.atts[att_num] > lo && row.atts[att_num] <= hi)
            cout << row << endl;
    }
    
    return CLStatus(0, 0);
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

CLTable* CLTable::GetInstance() {
    if(_pTable == 0)
        _pTable = new CLTable();
    
    return _pTable;
}