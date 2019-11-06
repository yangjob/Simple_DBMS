#include "CLMutex.h"
#include "CLTable.h"

CLMutex::CLMutex()
{
    int r = pthread_mutex_init(&_Mutex, 0);
    if(r != 0)
    {
        throw "在CLMutex::CLMutex()初始化构造中, 信号量初始化错误";
    }
}

CLMutex::~CLMutex()
{
    int r = pthread_mutex_destroy(&_Mutex);
    if(r != 0)
    {
        throw "在CLMutex::~CLMutex()析构函数中, 信号量销毁错误";
    }
}

CLStatus CLMutex::Lock()
{
    int r = pthread_mutex_lock(&_Mutex);
    if(r != 0)
    {
        return CLStatus(-1, 0);
    }
    else
    {
        return CLStatus(0, 0);
    }
}

CLStatus CLMutex::Unlock()
{
    int r = pthread_mutex_unlock(&_Mutex);
    if(r != 0)
    {
        return CLStatus(-1, 0);
    }
    else{
        return CLStatus(0, 0);
    }
}