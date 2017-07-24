#include <stdio.h>
#include <iostream>
#include "timer_queue.h"

using std::cout; 
using std::endl;

// check if occur time event
void CTimerQueue::ProcessExpire()
{
    int now = time(NULL);
    for(const auto &item : m_timerSet)
    {
        if(item == nullptr)
        {
            continue;
        }
        // occur time event
        int expire = item->GetExpire();
        if((expire > 0) && (expire <= now))
        {
            item->Run();
            item->UpdateExpire(now);
            //cout << "occur time event time: " << now << endl; 
        }
    }
}

// time event run at when
void CTimerQueue::RunAt(TimerCallback& cb, int when, int interval)
{
    CTimer* timer = new CTimer(cb, when, interval);
    m_timerSet.insert(timer);
    cout << "time now : " << time(NULL) << endl;
}

// time event run at after
void CTimerQueue::RunAfter(TimerCallback& cb, int delay, int interval)
{
    int when = time(NULL) + delay;
    RunAt(cb, when, interval);
}

