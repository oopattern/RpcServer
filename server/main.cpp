#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <vector>
#include <string>
#include <list>
#include <map>
#include <iostream>
#include <algorithm>
#include <functional>
#include "jsonintf.h"
#include "timer_queue.h"
#include "../component/udplog/log_udp.h"
#include "../component/net/event.h"
#include "../component/net/ae_epoll.h"

using std::list;
using std::string;
using std::vector;
using std::map;
using std::cout; 
using std::endl;
using namespace std::placeholders;

extern void TestJson(void);
extern int TestUdplog(void);

void TestVector()
{
    std::vector<int> v{1,2,3,4,5};
    cout << "Init vector:" << endl;
    // use reference
    for(auto &i : v)
    {
        i *= i;
    }
    // not use reference
    for(auto i : v)
    {
        cout << i << ' ';
    }
    cout << endl;
}

void TestUpper()
{
    cout << "- - - - - - - - Test1 - - - - - - - - - -" << endl;
    string s("upper");
    cout << "before: " << s << endl;
    if(s.begin() != s.end())
    {
        auto it = s.begin();
        *it = toupper(*it);
    }
    cout << "after: " << s << endl;

    cout << "- - - - - - - - Test2 - - - - - - - - - -" << endl;
    vector<string> v{"oath", "helloOmg", "saKule007"};
    for(auto it = v.begin(); it != v.end(); ++it)
    {
        for(auto& ch : *it)
        {
            ch = toupper(ch);
        }
    }
    for(auto item : v)
    {
        cout << item << endl;
    }
}

void TestIterator()
{
    // cbegin notice no change
    vector<string> v{"word","old","","new","age"};
    for(auto it = v.cbegin(); it != v.cend(); ++it)
    {
        if(it->empty())
        {
            cout << "find one empty" << endl;
            continue;
        }
        cout << *it << endl;
    }
}

void TestBinSearch()
{
    // notice bound
    int target = -1;
    vector<int> v{1,2,6,30,55,78,99,101,128};
    auto beg = v.cbegin(), end = v.cend();
    auto mid = beg + (end - beg) / 2;
    while(mid != end)
    {
        if(*mid == target)
        {
            cout << "find target " << target << endl;
            return;
        }
        else if(*mid > target)
        {
            end = mid;
        }
        else
        {
            beg = mid + 1; // find in next begin
        }
        mid = beg + (end - beg) / 2;
    }
    cout << "can not find the target" << endl;
}

void TestInsert()
{
    list<string> a{"haha","bbb","ccc","node"};
    vector<string> ins1{"a1","a2","a3","a4"};
    vector<string> ins2{"m11","m22","m33","m44"};
    cout << "- - - - - - - - - - - - - Insert Method 1 - - - - - - - - - - -" << endl;
    auto it = a.begin();
    for(auto& item : ins1)
    {
        it = a.insert(it, item);
    }
    for(auto item : a)
    {
        cout << item << ' ';
    }
    cout << endl;
    cout << "- - - - - - - - - - - - - Insert Method 2 - - - - - - - - - - -" << endl;
    for(auto& item : ins2)
    {
        a.push_front(item);
    }
    for(auto item : a)
    {
        cout << item << ' ';
    }
    cout << endl;
}

void TestErase()
{
    list<int> v{1,3,4,5,7,9,23,6,8,91};
    auto it = v.begin();
    while(it != v.end())
    {
        if((*it % 2) != 0)
        {
            it = v.erase(it);
        }
        else
        {
            ++it;
        }
    }
    for(const auto& item : v)
    {
        cout << item << ' ';
    }
    cout << endl;
}

void TestAlgorithm()
{
    vector<string> v{"oath","lidi","sakula","pattern","oath","dick","lidi"};
    for(const auto& item : v)
    {
        cout << item << ' ';
    }
    cout << endl;
    std::sort(v.begin(), v.end());
    auto uniend = std::unique(v.begin(), v.end());
    v.erase(uniend, v.end());
    for(const auto& item : v)
    {
        cout << item << ' ';
    }
    cout << endl;
}

void TestMap()
{
    vector<string> v{"oath","an","laji","work","money","laji","no","money","forbid","the","world"};
    list<string> ex{"a","an","the","no"};
    map<string, int> wordmap;
    for(const auto& item : v)
    {
        if(std::find(ex.cbegin(), ex.cend(), item) == ex.end())
        {
            ++wordmap[item];
        }
    }
    for(const auto& item : wordmap)
    {
        cout << item.first << " occur " << item.second << " times " << endl;
    }
}

bool CompareSize(string& s, int size)
{
    cout << s << endl;
    return (int)s.size() < size;
}

void TestBind()
{   
    class CBind 
    {
        public:
            bool CheckSize(string& s, int size)
            {
                return (int)s.size() < size;
            }
    };
    auto f = std::bind(CompareSize, _1, 6);
    string s("123456");
    cout << f(s) << endl;
}

int BindFunc(int arg1, std::string& arg2, float arg3)
{
    cout << "globalfunc:" << arg1 << " " << arg2 << " " << arg3 << endl;
    return 0;
}

typedef std::function<void (string&,float)> BIND_F1;
typedef std::function<void (int,int,int)> BIND_F2;
void TestBindFunction(void)
{
    class CBind 
    {
    public:
        int Check(int arg1, int arg2, int arg3)
        {
            cout << "memberfunc:" << arg1 << " " << arg2 << " " << arg3 << endl;
            return 0;
        }
    };
    BIND_F1 f1 = std::bind(BindFunc, 5566, _1, _2);
    std::string name("lidi");
    f1(name, 9.11);

    CBind bf;
    BIND_F2 f2 = std::bind(&CBind::Check, &bf, _3, _2, _1);
    f2(11,22,33);
}

void TestTimeCallback(string& name)
{
    cout << "Event1: " << time(NULL) << " name: " << name << endl;
}

void TestTimer()
{
    class CPerson
    {
    public:
        CPerson(string& name):m_firstname(name) 
        {

        }
        void PrintName(string& lastname)
        {
            cout << "Event2: " << time(NULL) <<  " FirstName: " << m_firstname << ", LastName: " << lastname << endl;
        }
    private:
        string m_firstname;
    };

    // use bind to register callback func
    string firstname("dick");
    string lastname("oopattern");
    CPerson person(firstname);
    CTimerQueue tq;
    //usage 1 : global func
    TimerCallback fg = std::bind(TestTimeCallback, lastname);
    tq.RunAfter(fg, 3, 0);
    //usage 2 : member func
    TimerCallback fm = std::bind(&CPerson::PrintName, &person, lastname);
    tq.RunAt(fm, time(NULL)+5, 2);

    // register epoll event
    aeEventLoop* pLoop = CreateEventLoop(MAX_EVENT_NUM);
    CreateFileEvent(pLoop, tq.GetTimerfd(), AE_READABLE, NULL, NULL);

    // loop event
    while(1)
    {
        int numevents = 0;
        struct timeval tvp;
        
        tvp.tv_sec = AE_WAIT_TIME;
        tvp.tv_usec = 0;
        numevents = aeApiPoll(pLoop, &tvp);
        if(numevents <= 0)
        {
            continue;
        }
        for(int i = 0; i < numevents; i++)
        {
            int fd = pLoop->acted[i].fd;
            int mask = pLoop->acted[i].mask;
            if((fd == tq.GetTimerfd()) 
                && (mask == AE_READABLE))
            {
                cout << "catch time event" << endl;
                tq.ProcessExpire();
            }
        }
    }
}

void TestSet()
{
    std::set<int> v{1,100,-3,66,9,18,11,23,57,48};
    for(const auto& item : v)
    {
        cout << item << endl;
    }

    std::set<std::pair<int,string>> xx{{100," hello "},{33," word "},{44," lidi "},{-8," listen "}};
    for(const auto& item : xx)
    {
        cout << item.first << item.second << endl;
    }
}

void TestMapCompare()
{
    cout << "- - - - - - - - - - - - - Test Map - - - - - - - - - - -" << endl;
    std::map<int,string> v{{1," www"},{-1," baidu"},{9," google"},{18," mtk"},{12," oath"}};
    for(const auto& item : v)
    {
        cout << item.first << item.second << endl;
    }
}

void TestMemFuncConst()
{
    return;   
}

void TestRefConst()
{
    class CWindow 
    {
    public:
        CWindow() {}
        virtual ~CWindow() {}
    public:
        virtual void Display() const 
        { 
            cout << "CWindow Display" << endl; 
        }
    };

    class CDerivedWindow: public CWindow
    {
    public:
        virtual void Display() const
        {
            cout << "CDerivedWindow Display" << endl;
        }
    };

    class CDisplay
    {
    public:
        void ConstValDisplay(CWindow w)
        {
            w.Display();
        }
        void ConstRefDisplay(const CWindow& w)
        {
            w.Display();
        }
    };

    CWindow baseW;
    CDerivedWindow derivedW;
    CDisplay d;
    cout << "- - - - - Test Const Pass - - - - - - " << endl;
    d.ConstValDisplay(baseW);
    d.ConstValDisplay(derivedW);
    cout << "- - - - - Test Const Reference - - - - - - " << endl;
    d.ConstRefDisplay(baseW);
    d.ConstRefDisplay(derivedW);
}

void TestRefReturn()
{
    class CRation
    {
    public:
        CRation(int num) : m_result(num) {}
        const CRation operator*(const CRation& w)
        {
            CRation out(w.m_result * this->m_result);
            return out;
        }   
        void GetVal() const
        {
            cout << m_result << endl;
        }
    private:
        int m_result;
    };

    CRation a(10);
    CRation b(25);
    CRation c = a * b;
    c.GetVal();
}

void FuncEach(int num)
{
    cout << num << endl;
}

void TestForeach()
{
    std::vector<int> v = {1,2,3,4,5};
    for_each(v.begin(), v.end(), std::bind(FuncEach, _1));
}

int main(void)
{
    TestUdplog();
    return 0;
}
