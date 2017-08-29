#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include "global.h"
#include "event.h"
#include "handle.h"

CClient::CClient(const char* name, int fd)
{
    m_cfd = fd;
    m_name = name;
    memset(m_querybuf, 0x0, sizeof(m_querybuf));
    m_nread = 0;
}

CClient::~CClient()
{
    
}

void CClient::OnConnected()
{
    printf("client fd[%d] connected success\n", m_cfd);
}

int CClient::ProcessQuery()
{
    // request from client
    m_nread = read(m_cfd, m_querybuf, sizeof(m_querybuf));
    if(m_nread == -1)
    {
        if(errno != EAGAIN)
        {
            DestroyClient(m_cfd);
        }
        printf("errno[%d]: %s\n", errno, strerror(errno));
        return -1;
    }
    else if(m_nread == 0)
    {
        DestroyClient(m_cfd);
        return 0;
    }
    m_querybuf[m_nread] = '\0';
    printf("recv:%s", m_querybuf);

    // reply to client
    char buf[AE_BUF_SIZE];
    snprintf(buf, sizeof(buf), "hello unix time[%d]\n", (int)time(NULL));
    SendToClient(buf, strlen(buf));

    return 0;
}

int CClient::SendToClient(const char* buf, int len)
{
    ssize_t nwritten = 0;

    if(len > (int)sizeof(m_replybuf))
    {
        len = sizeof(m_replybuf) - 1;
    }
    memcpy(m_replybuf, buf, len);
    m_replylen = len;

    if(m_replylen > 0)
    {
        nwritten = write(m_cfd, m_replybuf, m_replylen);
        if(nwritten == -1)
        {
            if(errno != EAGAIN)
            {              
                DestroyClient(m_cfd);
                printf("Error writing to client fd[%d]: %s", m_cfd, strerror(errno));
            }
        }
    }

    return 0;
}

int InitServer(int port)
{
    int s = -1;
    int yes = 1;
    int flags = 0;
    struct sockaddr_in sa;

    bzero(&sa, sizeof(sa));
    sa.sin_family = AF_INET;
    sa.sin_port = htons(port);

    if((s = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        close(s);
        return -1;
    }
    if(setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        close(s);
        return -1;
    }
    if(bind(s, (struct sockaddr*)&sa, sizeof(sa)) == -1)
    {
        close(s);
        return -1;
    }
    if(listen(s, 0) == -1)
    {
        close(s);
        return -1;
    }
    if((flags = fcntl(s, F_GETFL)) == -1)
    {
        close(s);
        return -1;
    }
    flags |= O_NONBLOCK;
    if(fcntl(s, F_SETFL, flags) == -1)
    {
        close(s);
        return -1;
    }

    return s;
}

int CreateClient(int cfd)
{
    if(cfd < 0)
    {
        return -1;
    }

    if(CreateFileEvent(CGlobalCfg::Instance()->m_pEventLoop, 
                cfd, AE_READABLE, HandleQuery, NULL) == -1)
    {
        printf("%s err\n", __FUNCTION__);
        close(cfd);
        return -1;
    }

    // check if already exeist
    if(CGlobalCfg::Instance()->m_clientMap.find(cfd) 
            != CGlobalCfg::Instance()->m_clientMap.end())
    {
        DestroyClient(cfd);
        printf("client fd[%d] already exixst! delete it\n", cfd);        
    }
    CClient* client = new CClient("", cfd);
    CGlobalCfg::Instance()->m_clientMap[cfd] = client;
    
    // connected
    client->OnConnected();
    return 0;
}

//不能只是简单close，还要删除注册的事件，否则epoll_ctl无法继续注册新的事件
void DestroyClient(int fd)
{
    ClientMap::iterator it = CGlobalCfg::Instance()->m_clientMap.find(fd);
    if(it != CGlobalCfg::Instance()->m_clientMap.end())
    {
        DeleteFileEvent(CGlobalCfg::Instance()->m_pEventLoop, fd, AE_READABLE);    
        DeleteFileEvent(CGlobalCfg::Instance()->m_pEventLoop, fd, AE_WRITABLE);
        close(fd);

        delete(it->second);
        CGlobalCfg::Instance()->m_clientMap.erase(it++);
        printf("client fd[%d], closed success\n", fd);
    }
}

int HandleQuery(aeEventLoop* eventLoop, int fd, void* clientData, int mask)
{
    AE_NOTUSED(eventLoop);
    AE_NOTUSED(clientData);
    AE_NOTUSED(mask);

    ClientMap::iterator it = CGlobalCfg::Instance()->m_clientMap.find(fd);
    if(it != CGlobalCfg::Instance()->m_clientMap.end())
    {
        (it->second)->ProcessQuery();
    }
    
    return 0;
}

int HandleAccept(aeEventLoop* eventLoop, int fd, void* clientData, int mask)
{
    int cfd = 0;
    struct sockaddr_in sa;
    socklen_t salen = 0;

    AE_NOTUSED(eventLoop);
    AE_NOTUSED(clientData);
    AE_NOTUSED(mask);

    while(1)
    {
        cfd = accept(fd, (struct sockaddr*)&sa, &salen);
        if(cfd == -1)
        {
            if(errno == EINTR)
            {
                continue;
            }
            else
            {
                return -1;
            }
        }
        CreateClient(cfd);
        return cfd;
    }
}


