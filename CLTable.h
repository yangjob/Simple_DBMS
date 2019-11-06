#ifndef CLTable_H
#define CLTable_H
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <string>
#include "CLStatus.h"
using namespace std;
typedef long long int64_t;
#define COLUMN_NUMS 100
#define MAX_ROWS 1000000

//表的元组（行）项
struct SRow{
public:
    SRow() { for(int i = 0; i < COLUMN_NUMS; i++) atts[i] = 0; }
    friend ostream &operator<<(ostream &output, const SRow &r) {
        for(int i = 0; i < COLUMN_NUMS; i++)
            output << r.atts[i] << " ";
        return output;
    }
    int64_t atts[COLUMN_NUMS];        //属性
};

//索引项
struct SIndexPair{
public:
    SIndexPair() { offset = -1; value = 0; }
    off_t offset;
    int64_t value;
};

//索引结构体
struct SIndex{
public:
    SIndex(string att, int att_index, string filename) :
        _att(att), _att_index(att_index), _filename(filename)  {
        _fd = open(_filename.c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
        _index_count = 0;
    }
    //向文件中写入新数据的索引
    CLStatus WriteIndex();
    //把文件中的特定索引删除
    CLStatus RemoveIndex();
    //从文件中读取全部索引
    CLStatus ReadIndex();

    string _att;                //索引的属性名称
    int _att_index;             //索引属性的下标
    string _filename;           //索引文件
    int _fd;                    //索引的文件描述符
    // map<off_t, int64_t> _index; //索引，key是文件偏移量，value是属性值
    SIndexPair _index[MAX_ROWS]; //索引，文件偏移量和属性值
    int _index_count;           //索引的数量，累积到一定程度写入文件
};

//数据表类，主类
class CLTable{
public:
    //构造函数
    CLTable();
    CLTable(string name, string filename, string atts_name[], string index_att);
    //查询搜索数据
    SRow* SelectReturn(string att_name, int64_t lo, int64_t hi);        //返回元组数组，未实现完毕，有bug
    CLStatus Select(string att_name, int64_t lo, int64_t hi);           //直接输出匹配的行，不返回SRow
    static CLStatus SelectData(string att_name, int64_t lo, int64_t hi);//不用创建对象就可以查询数据
    //插入数据
    CLStatus Insert();//暂未实现
    CLStatus InsertLast(SRow *row = NULL);
    static CLStatus InsertDataLast(SRow *row = NULL);   //不用创建对象就可以插入数据
    //删除数据，暂未实现
    CLStatus Delete();
    //索引
    int index();
    //析构函数
    virtual ~CLTable();
    //得到表对象 
    static CLTable* GetInstance();

private:
    CLTable(const CLTable&);
    CLTable& operator=(const CLTable&);
    static void OnProcessExit();
    static pthread_mutex_t* InitializeMutex();

private:
    string _name;                           //表名
    string _filename;                       //文件名
    int _fd;                                //文件描述符
    string _atts_name[COLUMN_NUMS];         //属性名
    static unsigned long row_nums;          //当前行数
    static CLTable* _pTable;                //用于GetInstance，全局只要一个表对象就可以了
    bool _bFlagProcessExit;                 //程序是否退出的标志

    SIndex* _index;                          //索引

    static pthread_mutex_t* _pMutexForCreatingTable;//多线程下创建表的互斥信号量
    pthread_mutex_t* _pMutexForInsert;      //多线程下插入操作的互斥信号量
};

#endif