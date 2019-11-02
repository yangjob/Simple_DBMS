#ifndef CLTable_H
#define CLTable_H
#include <iostream>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string>
#include "CLStatus.h"
using namespace std;
typedef long long int64_t;
#define COLUMN_NUMS 100

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

class CLTable{
public:
    //构造函数
    CLTable();
    CLTable(string name, string filename, string atts_name[]);
    //查询搜索数据
    SRow* SelectReturn(string att_name, int64_t lo, int64_t hi);        //返回元组数组，未实现完毕，有bug
    CLStatus Select(string att_name, int64_t lo, int64_t hi);           //直接输出匹配的行，不返回SRow
    static CLStatus SelectData(string att_name, int64_t lo, int64_t hi);//不用创建对象就可以查询数据
    //插入数据
    CLStatus Insert();
    CLStatus InsertLast(SRow *row = NULL);
    static CLStatus InsertDataLast(SRow *row = NULL);   //不用创建对象就可以插入数据
    //删除数据
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

private:
    void CreateTableFile();
    string _name;                           //表名
    string _filename;                       //文件名
    int _fd;                                //文件描述符
    string _atts_name[COLUMN_NUMS];         //属性名
    // SRow *_rows;                            //元组
    static const unsigned long max_rows = 1000000;  //最大行数
    static CLTable  *_pTable;               //用于GetInstance，全局只要一个表对象就可以了
};

#endif