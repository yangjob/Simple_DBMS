#include <iostream>
#include <unistd.h>
#include "CLTable.h"
// #include "CLThread.h"
// #include "CLExecutiveFunctionProvider.h"
using namespace std;


// struct SPara
// {
//     int Flag;
//     pthread_mutex_t mutex;
// };

// class CLMyFunction : public CLExecutiveFunctionProvider
// {
// public:
//     CLMyFunction()
//     {
//     }

//     virtual ~CLMyFunction()
//     {
//     }

//     virtual CLStatus RunExecutiveFunction(void *pContext)
//     {
//     SPara *p = (SPara*)pContext;

//     pthread_mutex_lock(&(p->mutex));

//     p->Flag++;

//     pthread_mutex_unlock(&(p->mutex));

//     return CLStatus(0, 0);
//     }
// };


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
    CLTable::SelectData("0", -1, 2);

    // CLExecutiveFunctionProvider *myfunction = new CLMyFunction();
    // CLExecutive *pThread = new CLThread(myfunction);

    // SPara *p = new SPara;
    // p->Flag = 3;
    // pthread_mutex_init(&(p->mutex), 0);

    // pThread->Run((void *)p);
    // // sleep(5);        //若没有这一步，则可能此线程先运行 ，输出4；否则必定创建的子线程先运行，最后输出5

    // pthread_mutex_lock(&(p->mutex));

    // p->Flag++;
    // cout << p->Flag << endl;

    // pthread_mutex_unlock(&(p->mutex));

    // pThread->WaitForDeath();

    // pthread_mutex_destroy(&(p->mutex));
    // delete p;

    // delete pThread;
    // delete myfunction;

    // return 0;
}