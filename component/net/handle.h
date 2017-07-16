#ifndef __HANDLE_H__
#define __HANDLE_H__

#include <string>
#include <map>
#include "event.h"

using namespace std;

class CClient;

typedef std::map<int, CClient*> ClientMap; // key:fd, val:point

class CClient
{
public:
    CClient(const char* name, int fd);    
    ~CClient();
    CClient(const CClient&);
    CClient& operator=(const CClient&);
public:
    void OnConnected();
    int ProcessQuery(); // recv from server
    int SendToClient(const char* buf, int len); // send to server
private:
    int m_cfd;
    string m_name;

    // recv buf
    char m_querybuf[AE_BUF_SIZE];
    int m_nread;

    // send buf
    char m_replybuf[AE_BUF_SIZE];
    int m_replylen;
};

// func
int InitServer(int port);
int CreateClient(int cfd);
void DestroyClient(int fd);
int HandleQuery(aeEventLoop* eventLoop, int fd, void* clientData, int mask);
int HandleAccept(aeEventLoop* eventLoop, int fd, void* clientData, int mask);

#endif

