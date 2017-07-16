#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include <string>
#include "event.h"
#include "handle.h"

using namespace std;

// single instance struct 
class CGlobalCfg {
public:
    static CGlobalCfg* Instance();
    void LoadCfg(const char* host, int port);
public:
    string m_host;
    int m_port;
    
    // event
    int m_fd; // listen fd
    aeEventLoop* m_pEventLoop;

    // client 
    ClientMap m_clientMap;
private:
    CGlobalCfg();
    ~CGlobalCfg();
    CGlobalCfg(const CGlobalCfg&);
    CGlobalCfg& operator=(const CGlobalCfg&);
private:
    static CGlobalCfg* m_pInstance;
};


#endif
