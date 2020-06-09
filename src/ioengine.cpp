#include "ioengine.h"
#include "isnet.h"
#include "logger.h"
#include "uv.h"

#include <sys/epoll.h>
#include <sys/time.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>

#define __CLASS__ "IOEngine"

IOEngine::IOEngine()
: m_pLoop(NULL)
{
    
}

IOEngine::~IOEngine() {
    if( m_pLoop ) {
        uv_loop_close(m_pLoop);
        m_pLoop = NULL;
    }
}

bool IOEngine::init()
{
    FUNLOG(Info, "ioengine init!!!", NULL);
    m_pLoop = uv_default_loop();
    uv_loop_init(m_pLoop);

    return true;
}

void IOEngine::run()
{
    FUNLOG(Info, "ioengine run!!!", NULL);
    uv_run(m_pLoop, UV_RUN_DEFAULT);
    m_bRunning = true;
}