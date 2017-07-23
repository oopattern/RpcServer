#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include "timer.h"

using std::cout;
using std::endl;

int CTimer::StartTimer(TimerCallback& cb, int nInterval)
{
    int cur = time(NULL);
    m_expireTimeList.push_front(cur+nInterval);
    m_callback = cb;
    cout << cur << ": start time" << endl;
    return 0;
}

int CTimer::StopTimer()
{
    return 0;
}

int CTimer::CheckExpire()
{
    // scan list to find out the expire time event
    int cur = time(NULL);
    auto it = m_expireTimeList.begin();
    while(it != m_expireTimeList.end())
    {
        if(cur < *it)
        {
            ++it;
            continue;
        }
        it = m_expireTimeList.erase(it);
        m_callback();
        cout << cur <<  ": occur timeout event hahah" << endl;
    }
    return 0;
}