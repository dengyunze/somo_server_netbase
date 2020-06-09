#include "env.h"
#include "logger.h"

#include <stdio.h>
#include <signal.h>
#include <sys/prctl.h>
#include <sys/resource.h>

bool Env::init()
{
    init_linux_env();
    init_sys_log();
    //return g_selector.init();

    return true;
}

bool Env::init_linux_env()
{
    ::signal(SIGHUP, SIG_IGN);

    ::signal(SIGPIPE, SIG_IGN);
    struct sigaction sig;
    sig.sa_handler = SIG_IGN;
    sig.sa_flags = 0;
    ::sigemptyset(&sig.sa_mask);
    ::sigaction(SIGPIPE,&sig,NULL);


    struct rlimit flimit;
    flimit.rlim_cur = 65535;
    flimit.rlim_max = 65535;
    if( ::setrlimit(RLIMIT_NOFILE, &flimit) < 0)
    {
        fprintf(stderr,"setrlimit error\n");
        return false;
    };


    flimit.rlim_cur = 2097152000-1;
    flimit.rlim_max = 2097152000-1;
    if( ::setrlimit(RLIMIT_CORE, &flimit) < 0)
    {
        fprintf(stderr,"setrlimit2 error\n");
        return false;
    };

    ::prctl(PR_SET_DUMPABLE, 1);
    return true;
}


