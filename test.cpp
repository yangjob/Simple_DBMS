#include <iostream>
#include <fcntl.h>
#include "CLTable.h"
using namespace std;

int main()
{
    SRow row;
    for(int i = 0; i < COLUMN_NUMS; i++) row.atts[i] = i;
    //测试添加数据
    CLStatus s = CLTable::InsertDataLast(&row);
    //测试错误处理类
    if(s.IsSuccess()) cout << "成功添加数据" << endl;
    else cout << "添加数据失败" << endl;
    //测试查询搜索
    CLTable::SelectData("50", 49, 52);

    return 0;
}