#pragma once

#include "isnet.h"

struct uv_timer_s;
struct ITimerHandler;
class Timer : public ISNTimer
{
public:
    Timer();
    ~Timer();

public:
    virtual void    init(int id);
    virtual void    set_handler(ISNTimerHandler* handler);
    virtual void    start(int interval);
    virtual void    stop();
    virtual void    close();

private:
    static void    on_timer(uv_timer_s* handle);

private:
    uv_timer_s* m_pTimer;
    ISNTimerHandler*  m_pHandler;
    int         m_nId;
};

