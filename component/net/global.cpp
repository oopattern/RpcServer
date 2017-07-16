#include <stdio.h>
#include <stdlib.h>
#include "global.h"

#define TEST_DEBUG 0

// init instance
CGlobalCfg* CGlobalCfg::m_pInstance = NULL;

// create global
CGlobalCfg* CGlobalCfg::Instance()
{
    if(NULL == m_pInstance)
    {
        m_pInstance = new CGlobalCfg;
    }
    return m_pInstance;
}

CGlobalCfg::CGlobalCfg()
{
    m_host = "";
    m_port = 0;
}

CGlobalCfg::~CGlobalCfg()
{

}

void CGlobalCfg::LoadCfg(const char* host, int port)
{
    m_host = host;
    m_port = port;
}

#if TEST_DEBUG
int main(void)
{
    CGlobalCfg::Instance()->LoadCfg("192.168.1.42", 8888);
    printf("ip[%s] port[%d]\n", CGlobalCfg::Instance()->m_host.c_str(), CGlobalCfg::Instance()->m_port);
    
    return 0;
}
#endif

