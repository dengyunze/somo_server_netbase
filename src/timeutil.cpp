#include "timeutil.h"

#include <sys/time.h>
#include <memory.h>
#include <string>

using namespace somo::video;

uint32_t timeutil::sys_time_sec()
{
    return ::time(NULL);
}

uint64_t timeutil::sys_time_msec()
{
    struct timeval _tv;
    ::gettimeofday(&_tv, NULL);
    return (_tv.tv_sec*1000) + (_tv.tv_usec/1000);
}


