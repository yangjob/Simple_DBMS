#ifndef CLSTATUS_H
#define CLSTATUS_H

//出错处理类
class CLStatus
{
public:
    //lReturnCode>=0表示没有出错
    CLStatus(long lReturnCode, long lErrorCode);
    CLStatus(const CLStatus& s);
    virtual ~CLStatus();

public:
    bool IsSuccess();

public:
    const long& m_clReturnCode;
    const long& m_clErrorCode;

private:
    long m_lReturnCode;
    long m_lErrorCode;
};

#endif