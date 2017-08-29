#ifndef __TIMER_QUEUE_H__
#define  __TIMER_QUEUE_H__
#include <unistd.h>
#include <string.h>
#include <set>
#include <vector>
#include "timer.h"

using std::set;

// key: expiration, can not use map, since key(expiretion) may be the same
typedef std::pair<int,CTimer*> TimeEntry;
typedef std::set<TimeEntry> TimerSet;

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
