#include <stdio.h>
#include <iostream>
#include <sys/timerfd.h>
#include "timer_queue.h"

using std::cout; 
using std::endl;

int CreateTimerfd()
{
    int tmfd = ::timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK | TFD_CLOEXEC);
    if(tmfd < 0)
    {
        cout << "Fail in timerfd_create" << endl;
    }
    return tmfd;
}

void ReadTimerfd(int timerfd)
{
    uint64_t howmany;
    ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
    if(n != sizeof(howmany))
    {
        cout << "Readtimerfd reads" << n << " instead of 8 bytes";
    }
}

void ResetTimerfd(int timerfd, int expire)
{
    struct itimerspec newVal;
    struct itimerspec oldVal;
    bzero(&newVal, sizeof(newVal));
    bzero(&oldVal, sizeof(oldVal));
    struct timespec ts;
    ts.tv_sec = expire - time(NULL);
    ts.tv_nsec = 0;
    newVal.it_value = ts;
    int ret = ::timerfd_settime(timerfd, 0, &newVal, &oldVal);
    if(ret)
    {
        cout << "Fail in timerfd_settime" << endl;
    }
}

CTimerQueue::CTimerQueue()
{
    m_timerSet.clear();
    m_timerfd = CreateTimerfd();
}

// check if occur time event
void CTimerQueue::ProcessExpire()
{
    // first, need to read timerfd
    ReadTimerfd(m_timerfd);

    // second, find expire timer
    int now = time(NULL);
    std::vector<TimeEntry> expire;
    TimeEntry sentry(now, reinterpret_cast<CTimer*>(UINTPTR_MAX));
    auto itbeg = m_timerSet.begin();
    auto itend = m_timerSet.lower_bound(sentry);
    std::copy(itbeg, itend, back_inserter(expire));
    // already timeout, clean it
    m_timerSet.erase(itbeg, itend);

    // handle timeout event
    for(auto& item : expire)
    {
        (item.second)->Run();
        cout << "occur time event time: " << now << endl;
    }

    // update next timeout event
    for(auto& item : expire)
    {
        if(item.second->IsRepeat())
        {
            item.second->Restart(now);
            m_timerSet.insert(TimeEntry((item.second)->GetExpire(),item.second));      
        }
        else
        {
            delete item.second;
        }
    }

    // check if need to update timerfd_xxx
    if(!m_timerSet.empty())
    {
        auto it = m_timerSet.begin();
        if(it->second->IsValid())
        {
            ResetTimerfd(m_timerfd, it->second->GetExpire());
        }
    }
}

// time event run at when
void CTimerQueue::RunAt(TimerCallback& cb, int when, int interval)
{
    CTimer* timer = new CTimer(cb, when, interval);
    // first, check if need to update timefd_xxx
    auto itbeg = m_timerSet.begin();
    if((itbeg == m_timerSet.end()) || (when < itbeg->first))
    {
        ResetTimerfd(m_timerfd, when);
    }
    // after, inset timer
    m_timerSet.insert(TimeEntry(when,timer));
    cout << "time now : " << time(NULL) << endl;
}

// time event run at after
void CTimerQueue::RunAfter(TimerCallback& cb, int delay, int interval)
{
    int when = time(NULL) + delay;
    RunAt(cb, when, interval);
}

