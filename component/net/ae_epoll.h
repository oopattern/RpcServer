#ifndef __AE_EPOLL_H__
#define __AE_EPOLL_H_

#include "event.h"

typedef struct aeApiState {
    int epfd;
    struct epoll_event *events;
} aeApiState;

int aeApiCreate(aeEventLoop *eventLoop);
int aeApiResize(aeEventLoop *eventLoop, int setsize); 
void aeApiFree(aeEventLoop *eventLoop); 
void aeApiFree(aeEventLoop *eventLoop); 
int aeApiAddEvent(aeEventLoop *eventLoop, int fd, int mask);
void aeApiDelEvent(aeEventLoop *eventLoop, int fd, int delmask);
int aeApiPoll(aeEventLoop *eventLoop, struct timeval *tvp);
char *aeApiName(void);


#endif
