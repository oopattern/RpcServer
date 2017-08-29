#ifndef __TIMER_H__
#define  __TIMER_H__
#include <iostream>
#include <functional>

typedef std::function<void()> TimerCallback;

class CTimer
{
public:
    CTimer(TimerCallback& cb, int when, int interval) 
        : m_callback(cb), m_expireSec(when), m_interval(interval)
    {

    }
    void Run();
    int GetExpire()
    {
        return m_expireSec;
    }
    bool IsRepeat()
    {
        return ((m_interval != 0) ? true : false);
    }
    bool IsValid()
    {
        return ((m_expireSec != 0) ? true : false);
    }
    void Restart(int now)
    {
        m_expireSec = now + m_interval;
    }
private:
    TimerCallback m_callback;
    int m_expireSec;
    int m_interval;
};

#endif
