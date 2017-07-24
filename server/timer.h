#ifndef __TIMER_H__
#define  __TIMER_H__
#include <iostream>

typedef std::function<void()> TimerCallback;

class CTimer
{
public:
    CTimer(TimerCallback& cb, int when, int interval) 
        : m_callback(cb), m_expireSec(when), m_interval(interval)
    {

    }
    void Run();
    void UpdateExpire(int now)
    {
        m_expireSec = ((0 == m_interval) ? (0) : (now + m_interval));
    }
    int GetExpire()
    {
        return m_expireSec;
    }
private:
    TimerCallback m_callback;
    int m_expireSec;
    int m_interval;
};

#endif
