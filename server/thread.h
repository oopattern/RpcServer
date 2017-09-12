#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <deque>
#include <string>
#include <errno.h>
#include <pthread.h>
#include <functional>

using std::string;
using std::vector;
using std::deque;
using std::cout; 
using std::endl;
using namespace std::placeholders;

typedef std::function<void ()> ThreadFunc;
const int MSG_MAX_NUM = 100;


struct ThreadData
{
    ThreadFunc m_func;
    std::string m_name;
    ThreadData(const ThreadFunc& func, const std::string& name)
        : m_func(func), m_name(name)
    {
    
    }

    void RunInThread()
    {
        m_func();
    }
};

void* StartThread(void *arg)
{
    struct ThreadData* pData = static_cast<ThreadData*>(arg);
    pData->RunInThread();
    delete pData;
    //cout << "Thread:" << pData->m_name << " end " << endl;
    return (void*)0;
}

class CThread
{
public:
    CThread(const ThreadFunc& func, const std::string& name) 
        : m_func(func), m_name(name)
    {
        SetDefaultName();       
    }

    void Start()
    {
        struct ThreadData* pData = new ThreadData(m_func, m_name);
        if (0 != pthread_create(&m_tid, NULL, StartThread, pData))
        {
            int errcode = errno;
            cout << strerror(errcode) << endl;
        }
    }

    void Join()
    {
        pthread_join(m_tid, NULL);
    }

    const std::string& GetThreadName() const
    {
        return m_name;
    }

    pid_t GetPid()
    {
        return getpid();
    }
private:
    ThreadFunc m_func;
    std::string m_name;
    pthread_t m_tid;
    static int s_numCreated;

    void SetDefaultName()
    {
        if (m_name.empty())
        {
            char buf[64];
            snprintf(buf, sizeof(buf), "Thread%d", ++s_numCreated);
            m_name = buf;
        }
    }
};

class CTestThread
{
public:
    CTestThread(int num) 
        : m_mutex(PTHREAD_MUTEX_INITIALIZER),
          m_cond(PTHREAD_COND_INITIALIZER)
    {
        cout << "main thread tid: " <<  ::pthread_self() << endl;
        for (int i = 0; i < num; i++)
        {
            char name[64];
            snprintf(name, sizeof(name), "work_thread%d", i+1);
            m_threads.push_back(new CThread(std::bind(&CTestThread::Consume, this), name));
        }
        for_each(m_threads.begin(), m_threads.end(), std::bind(&CThread::Start, _1));
    }

    void JoinAll()
    {
        for (size_t i = 0; i < m_threads.size(); i++)
        {
            pthread_mutex_lock(&m_mutex);
            m_msg.push_back("stop");
            pthread_mutex_unlock(&m_mutex);
        }
        for_each(m_threads.begin(), m_threads.end(), std::bind(&CThread::Join, _1));
    }

    void Product(int msgNum)
    {
        for (int i = 0; i < msgNum; i++)
        {
            pthread_mutex_lock(&m_mutex);
            if (m_msg.empty())
            {
                pthread_cond_broadcast(&m_cond);
            }
            char buf[32];
            snprintf(buf, sizeof(buf), "hello %d", i+1);
            m_msg.push_back(buf);
            cout << "main thread tid: " << ::pthread_self() <<  " put msg: " << buf << endl;
            pthread_mutex_unlock(&m_mutex);
        }
    }

    void Consume()
    {
        bool running = true;
        while (running)
        {
            pthread_mutex_lock(&m_mutex);
            while (m_msg.empty())
            {
                pthread_cond_wait(&m_cond, &m_mutex);
            }
            std::string item = m_msg.front();
            m_msg.pop_front();
            cout << "work thread tid: " << ::pthread_self() << " get msg: " << item << endl;
            pthread_mutex_unlock(&m_mutex);
            running = (item != "stop");
        }
    }
private:
    pthread_mutex_t m_mutex;
    pthread_cond_t m_cond;
    std::deque<std::string> m_msg;
    std::vector<CThread*> m_threads;
};

