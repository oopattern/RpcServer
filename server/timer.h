#ifndef __TIMER_H__
#define  __TIMER_H__

#include <iostream>
#include <list>
#include <string>

using std::list;
using std::string;

typedef std::function<void()> TimerCallback;

class CTimer
{
public:
    int StartTimer(TimerCallback& cb, int nInterval);
    int StopTimer();
    int CheckExpire();
private:
    TimerCallback m_callback;
    list<int> m_expireTimeList;
};

#endif
