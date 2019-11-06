#ifndef CLEXECUTIVE_H
#define CLEXECUTIVE_H

#include "CLStatus.h"

class CLExecutiveFunctionProvider;  //(https://blog.csdn.net/libing_zeng/article/details/80875052)，两个文件内的A、B类互相使用时的解决方法

//执行类，给出运行CLExecutiveFunctionProvider类或其派生类中提供的函数的接口
class CLExecutive
{
public:
    //显式构造函数，不使用默认的构造函数，即创建对象时必须制定参数
    explicit CLExecutive(CLExecutiveFunctionProvider *pExecutiveFunctionProvider);
    //虚析构函数，让各自创建的线程类可以各自清理属于自己的资源如指针指向内存等
    virtual ~CLExecutive();

    //运行CLExecutiveFunctionProvider类及其派生类提供的函数
    virtual CLStatus Run(void *pContext = 0) = 0;
    //等待（创建的线程）结束/死亡
    virtual CLStatus WaitForDeath() = 0;

protected:
    CLExecutiveFunctionProvider *_pExecutiveFunctionProvider;

private:
    CLExecutive(const CLExecutive&);
    CLExecutive& operator=(const CLExecutive&);
};

#endif