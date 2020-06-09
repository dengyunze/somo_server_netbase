#include "netaddr.h"

#include <arpa/inet.h>
#include <sys/time.h>
#include <memory.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <unistd.h>

uint32_t netaddr::get_host_ip(const char* hostname)
{
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL)
    {
        return 0;
    }

    return *(u_long *)host->h_addr;
}

std::string netaddr::get_host_name()
{
    char temp[255];
    if (gethostname(temp, 255)) {
        return "";
    }
    return temp;
}

uint32_t netaddr::aton(const std::string& ip)
{
    return inet_addr( ip.c_str() );
}

std::string netaddr::ntoa(uint32_t ip)
{
    struct in_addr addr;
    memcpy(&addr, &ip, 4);
    return std::string(::inet_ntoa(addr));
}


