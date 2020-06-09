#pragma once
#include <sys/syslog.h>

enum
{
    Emerg = LOG_EMERG,
    Error = LOG_ERR,
    Warn = LOG_WARNING,
    Notice = LOG_NOTICE,
    Info = LOG_INFO,
    Debug = LOG_DEBUG
};

void init_sys_log();
void log(int level, const char *fmt, ...);
void set_syslog_level(int level);

#define FUNLOG(level, fmt, ...)   log(level, "[%s::%s]:" fmt,  __CLASS__, __FUNCTION__, __VA_ARGS__)
