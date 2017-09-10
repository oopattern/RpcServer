#ifndef __TIMER_QUEUE_H__
#define  __TIMER_QUEUE_H__
#include <unistd.h>
#include <string.h>
#include <set>
#include <vector>
#include <iostream>
#include <functional>

using std::set;

class CTimer;
// key: expiration, can not use map, since key(expiretion) may be the same
typedef std::function<void()> TimerCallback;
typedef std::pair<int,CTimer*> TimeEntry;
typedef std::set<TimeEntry> TimerSet;


class CTimer
{
public:
    CTimer(TimerCallback& cb, int when, int interval) 
        : m_callback(cb), m_expireSec(when), m_interval(interval)
    {

    }
    void Run()
    {
        m_callback();
    }
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


class CTimerQueue
{
public:
    CTimerQueue();
    int  GetTimerfd() { return m_timerfd; }
    void ProcessExpire();
    void RunAt(TimerCallback& cb, int when, int interval);
    void RunAfter(TimerCallback& cb, int delay, int interval);
private:
    TimerSet m_timerSet;
    int m_timerfd; // use linux timefd_xxx, with epoll, better than time wheel
};

#endif
