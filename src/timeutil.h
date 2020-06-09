#pragma once

#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>

class timeutil
{
public:
    static uint32_t     sys_time_sec();
    static uint64_t     sys_time_msec();
};


