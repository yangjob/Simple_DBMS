#ifndef CLTHREAD_H
#define CLTHREAD_H
#include <iostream>
#include <pthread.h>
#include "CLStatus.h"
#include "CLExecutive.h"

class CLThread : public CLExecutive{
public:
    //显式构造函数，不使用默认的构造函数，即创建对象时必须制定参数
    explicit CLThread(CLExecutiveFunctionProvider *pExecutiveFunctionProvider);
    //虚析构函数，让各自创建的线程类可以各自清理属于自己的资源如指针指向内存等
    virtual ~CLThread();        

    //创建线程，让其运行CLExecutiveFunctionProvider类及其派生类提供的函数
    virtual CLStatus Run(void* pContext = NULL);
    //等待线程结束/死亡
    virtual CLStatus WaitForDeath();

private:
    //线程入口函数
    static void* StartFunctionOfThread(void *pContext);

private:
    void* _pContext;            //比较自由的指针，这里指向一个对象的线程
    pthread_t _threadID;        //存储创建的线程的pid
};

#endif