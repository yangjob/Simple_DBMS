#include "CLThread.h"
#include "CLExecutiveFunctionProvider.h"
using namespace std;


//显式构造函数
CLThread::CLThread(CLExecutiveFunctionProvider *pExecutiveFunctionProvider) : CLExecutive(pExecutiveFunctionProvider) {
    _pContext = 0;
}


//创建线程，让其运行CLExecutiveFunctionProvider类及其派生类提供的函数
CLStatus CLThread::Run(void *pContext) {
    _pContext = pContext;

    int r = pthread_create(&_threadID, 0, StartFunctionOfThread, this);
    if(r != 0) {//线程创建失败
        /*  */
        return CLStatus(-1, 0);
    }

    return CLStatus(0, 0);
}


//等待线程死亡
CLStatus CLThread::WaitForDeath() {
    int r = pthread_join(_threadID, 0);
    if(r != 0) {
        /*  */
        return CLStatus(-1, 0);
    }

    return CLStatus(0, 0);
}


//线程入口函数
void* CLThread::StartFunctionOfThread(void *pContext) {
    CLThread* pContextThread = (CLThread *)pContext;
    CLStatus s = pContextThread->_pExecutiveFunctionProvider->RunExecutiveFunction(pContextThread->_pContext);

    return (void *)s.m_clReturnCode;
}


//析构函数
CLThread::~CLThread() {

}
