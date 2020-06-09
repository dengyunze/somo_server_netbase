#include "logger.h"
#include <stdio.h>
#include <stdarg.h>
#include <sys/time.h>

int g_log_level = Info; //default
struct timeval g_tv;

void init_sys_log()
{
    ::openlog(NULL, LOG_PID, LOG_LOCAL0);
}

void log(int level, const char *fmt, ...)
{
    if ( level > g_log_level )
        return;

    va_list    param;

    ::va_start(param, fmt);
    ::vsyslog(level, fmt, param);
    ::va_end(param);
}

void set_syslog_level(int level)
{
    g_log_level = level;
}
