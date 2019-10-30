#ifndef CLTable_H
#define CLTable_H
#include <string>
using namespace std;
typedef long long int64_t;
#define COLUMN_NUMS 100

struct SRow{
public:
    int64_t atts[COLUMN_NUMS];        //属性
};

class CLTable{
public:
    //构造函数
    CLTable();
    CLTable(string name, string filename, string *atts_name);
    //查询搜索数据
    SRow* Select(string att_name, int64_t lo, int64_t hi);
    //插入数据
    void Insert();
    void InsertLast();
    //删除数据
    void Delete();
    //索引
    int index();

private:
    string _name;                           //表名
    string _filename;                       //文件名
    string _atts_name[COLUMN_NUMS];         //属性名
    SRow *_rows;                             //元组
    unsigned const int max_rows = 1000000;  //最大行数
};

#endif