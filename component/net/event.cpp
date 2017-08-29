#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include "event.h"
#include "ae_epoll.h"

#define TEST_DENUG 0

// static func
static int ProcessFileEvent(aeEventLoop* eventLoop);
static int ProcessTimeEvent(aeEventLoop* eventLoop);

aeEventLoop* CreateEventLoop(int setsize)
{
    aeEventLoop* eventLoop = NULL;

    if((eventLoop = (aeEventLoop*)malloc(sizeof(*eventLoop))) == NULL)
    {
        goto err;
    }
    eventLoop->events = (aeFileEvent*)malloc(sizeof(aeFileEvent)*setsize);
    eventLoop->acted = (aeActedEvent*)malloc(sizeof(aeActedEvent)*setsize);
    if((eventLoop->events == NULL) || (eventLoop->acted == NULL)) 
    {
        goto err;
    }
    eventLoop->maxfd = -1;
    eventLoop->setsize = setsize;
    //eventLoop->timeEventHead = NULL;
    for(int i = 0; i < setsize; i++)
    {
        eventLoop->events[i].mask = AE_NONE;
    }
    if(aeApiCreate(eventLoop) == -1)
    {
        goto err;
    }
    
    return eventLoop;

err:
    if(eventLoop)
    {
        free(eventLoop->events);
        free(eventLoop);
    }
    return NULL;
}

int CreateFileEvent(aeEventLoop* eventLoop, int fd, int mask, aeFileProc* proc, void* clientData)
{
    if(eventLoop == NULL)
    {
        return AE_ERR;
    }

    if(fd >= eventLoop->setsize)
    {
        return AE_ERR;
    }

    if(aeApiAddEvent(eventLoop, fd, mask) == -1)
    {
        return AE_ERR;
    }

    aeFileEvent* fe = &eventLoop->events[fd];
    fe->mask |= mask;
    if(mask & AE_READABLE)
    {
        fe->rfileProc = proc;
    }
    if(mask & AE_WRITABLE)
    {
        fe->wfileProc = proc;
    }
    fe->clientData = clientData;
    if(fd > eventLoop->maxfd)
    {
        eventLoop->maxfd = fd;
    }

    return AE_OK;
}

void DeleteFileEvent(aeEventLoop* eventLoop, int fd, int mask)
{
    if(fd > eventLoop->setsize)
    {
        return;
    }

    aeFileEvent* fe = &eventLoop->events[fd];
    if(fe->mask == AE_NONE)
    {
        return;
    }

    aeApiDelEvent(eventLoop, fd, mask);
    fe->mask &= (~mask);

    // update new maxfd
    if((eventLoop->maxfd == fd) && (fe->mask == AE_NONE))
    {
        int newmax = -1;
        for(newmax = eventLoop->maxfd-1; newmax >= 0; newmax--)
        {
            if(eventLoop->events[newmax].mask != AE_NONE)
            {
                break;
            }
        }
        eventLoop->maxfd = newmax;
    }
}

int ProcessEvent(aeEventLoop* eventLoop)
{
    ProcessFileEvent(eventLoop);
    ProcessTimeEvent(eventLoop);
    return 0;
}

int ProcessFileEvent(aeEventLoop* eventLoop)
{
    int numevents = 0;
    struct timeval tvp;
    
    tvp.tv_sec = AE_WAIT_TIME;
    tvp.tv_usec = 0;
    numevents = aeApiPoll(eventLoop, &tvp);
    if(numevents <= 0)
    {
        return 0;
    }

    for(int i = 0; i < numevents; i++)
    {
        int fd = eventLoop->acted[i].fd;
        int mask = eventLoop->acted[i].mask;
        aeFileEvent* fe = &eventLoop->events[fd];
        
        // if need to clear mask?
        if(fe->mask 
            && (mask & AE_READABLE)
            && (fe->rfileProc != NULL))
        {
            fe->rfileProc(eventLoop, fd, fe->clientData, mask);
        }
        if(fe-mask 
            && (mask & AE_WRITABLE)
            && (fe->wfileProc != NULL))
        {
            fe->wfileProc(eventLoop, fd, fe->clientData, mask);
        }
    }
    return numevents;
}

int ProcessTimeEvent(aeEventLoop* eventLoop)
{
    return 0;
}

void MainLoop(aeEventLoop* eventLoop)
{
    while(1)
    {
        ProcessEvent(eventLoop);
        //printf("time: %d\n", time(NULL));
    }
}

#if TEST_DENUG
int main(void)
{
    printf("hello world\n");

    aeEventLoop* eventLoop = CreateEventLoop(MAX_EVENT_NUM);
    if(eventLoop == NULL)
    {
        return -1;
    }
    MainLoop(eventLoop);

    return 0;
}
#endif

