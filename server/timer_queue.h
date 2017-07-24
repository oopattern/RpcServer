#ifndef __TIMER_QUEUE_H__
#define  __TIMER_QUEUE_H__
#include <set>
#include "timer.h"

using std::set;

// key: expiration, can not use map, since key(expiretion) may be the same
typedef set<CTimer*> TimerSet;

class CTimerQueue
{
public:
    void ProcessExpire();
    void RunAt(TimerCallback& cb, int when, int interval);
    void RunAfter(TimerCallback& cb, int delay, int interval);
private:
    TimerSet m_timerSet;
};

#endif