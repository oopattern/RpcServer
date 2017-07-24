#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <vector>
#include <string>
#include <list>
#include <iostream>
#include <algorithm>
#include <functional>
#include "jsonintf.h"
#include "timer_queue.h"

using std::list;
using std::string;
using std::vector;
using std::map;
using std::cout; 
using std::endl;
using namespace std::placeholders;

extern void TestJson(void);

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
    return s.size() < size;
}

void TestBind()
{   
    class CBind 
    {
        public:
            bool CheckSize(string& s, int size)
            {
                return s.size() < size;
            }
    };
    auto f = std::bind(CompareSize, _1, 6);
    string s("123456");
    cout << f(s) << endl;
}

void TestTimeCallback(string& name)
{
    cout << "hello " << name << ": timer event callback" << endl;
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
            cout << "FirstName: " << m_firstname << ", LastName: " << lastname << endl;
        }
    private:
        string m_firstname;
    };

    string firstname("oath");
    string lastname("lidi");
    CPerson person(firstname);
    CTimerQueue tq;
    //usage 1 : global func
    //TimerCallback f = std::bind(TestTimeCallback, lastname);
    //usage 2 : member func
    TimerCallback func = std::bind(&CPerson::PrintName, &person, lastname);
    tq.RunAt(func, time(NULL)+5, 2);
    while(1)
    {
        tq.ProcessExpire();
        sleep(1);
    }
}

int main(void)
{
    //TestJson();
    //TestVector();
    //TestUpper();
    //TestIterator();
    //TestBinSearch();
    //TestInsert();
    //TestErase();
    //TestAlgorithm();
    //TestMap();
    //TestBind();
    TestTimer();
    return 0;
}
