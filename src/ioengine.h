#pragma once

#include <stdint.h>
#include <set>

struct uv_loop_s;
struct FdBase;
class IOEngine
{
public:
    IOEngine();
    ~IOEngine();

public:
    bool init();
    void run();

private:
    uv_loop_s*  m_pLoop;
    bool        m_bRunning;
};

