#include "timer.h"

#include "logger.h"
#include "uv.h"
#include <malloc.h>

#define __CLASS__ "Timer"

Timer::Timer() 
: m_pTimer(NULL)
, m_nId(0)
{
    m_pTimer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
    m_pTimer->data = this;
    uv_timer_init(uv_default_loop(), m_pTimer);
}

Timer::~Timer() {
    if( m_pTimer ) {
        uv_close((uv_handle_t*)m_pTimer, NULL);
        m_pTimer = NULL;
    }
}

void    Timer::init(int id) {
    m_nId = id;
}

void    Timer::set_handler(ISNTimerHandler* handler) {
    m_pHandler = handler;
}
 
void    Timer::start(int interval) {
    uv_timer_start(m_pTimer, on_timer, 0, interval);
}

void    Timer::stop() {
    uv_timer_stop(m_pTimer);
}

void    Timer::close() {
    if( m_pTimer ) {
        uv_close((uv_handle_t*)m_pTimer, NULL);
        m_pTimer = NULL;
    }
}

void    Timer::on_timer(uv_timer_t* handle) {
    //FUNLOG(Info, "tcp link timer!!!", NULL);
    Timer* timer = (Timer*)handle->data;
    if( timer == NULL ) {
        FUNLOG(Error, "timer failed, timer==NULL", NULL);
        return;
    }

    if( timer->m_pHandler != NULL ) {
        timer->m_pHandler->on_timer(timer->m_nId);
    }
}