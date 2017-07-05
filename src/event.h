#ifndef __EVENT_H__
#define __EVENT_H__

// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - -  
#define AE_WAIT_TIME 2 
#define AE_BUF_SIZE 1024
#define MAX_EVENT_NUM (1024*10)

#define AE_OK 0
#define AE_ERR -1

#define AE_NONE 0
#define AE_READABLE 1
#define AE_WRITABLE 2

#define AE_FILE_EVENTS 1
#define AE_TIME_EVENTS 2
#define AE_ALL_EVENTS (AE_FILE_EVENTS|AE_TIME_EVENTS)
#define AE_DONT_WAIT 4

/* Macros */
#define AE_NOTUSED(V) ((void) V)
// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -  - - - - - - - - - - - 

struct aeEventLoop;

/* Types and data structures */
typedef int aeFileProc(struct aeEventLoop *eventLoop, int fd, void *clientData, int mask);

/* File event structure */
typedef struct aeFileEvent {
    int mask; /* one of AE_(READABLE|WRITABLE) */
    aeFileProc *rfileProc;
    aeFileProc *wfileProc;
    void *clientData;
} aeFileEvent;

/* A fired event */
typedef struct aeActedEvent {
    int fd;
    int mask;
} aeActedEvent;

/* State of an event based program */
typedef struct aeEventLoop {
    int maxfd;   /* highest file descriptor currently registered */
    int setsize; /* max number of file descriptors tracked */
    aeFileEvent *events; /* Registered events */
    aeActedEvent *acted;
    //aeTimeEvent *timeEventHead;
    void *apidata; /* This is used for polling API specific data */
} aeEventLoop;

aeEventLoop* CreateEventLoop(int setsize);
int CreateFileEvent(aeEventLoop* eventLoop, int fd, int mask, aeFileProc* proc, void* clientData);
void DeleteFileEvent(aeEventLoop* eventLoop, int fd, int mask);
int ProcessEvent(aeEventLoop* eventLoop);
void MainLoop(aeEventLoop* eventLoop);

#endif
