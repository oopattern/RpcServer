#include <stdio.h>
#include <stdlib.h>
#include "global.h"
#include "event.h"
#include "handle.h"


int main(void)
{
    CGlobalCfg::Instance()->LoadCfg("127.0.0.1", 9600);
    CGlobalCfg::Instance()->m_fd = InitServer(CGlobalCfg::Instance()->m_port);
    CGlobalCfg::Instance()->m_pEventLoop = CreateEventLoop(MAX_EVENT_NUM);
    if((CGlobalCfg::Instance()->m_fd == -1) || (CGlobalCfg::Instance()->m_pEventLoop == NULL))
    {
        return -1;
    }
    CreateFileEvent(CGlobalCfg::Instance()->m_pEventLoop, CGlobalCfg::Instance()->m_fd,
            AE_READABLE, HandleAccept, NULL);
    MainLoop(CGlobalCfg::Instance()->m_pEventLoop);

    return 0;
}

