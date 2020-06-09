#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include "packet.h"

namespace somo{
namespace video{

class netaddr
{
public:
    static uint32_t        get_host_ip(const char* hostname);
    static std::string     get_host_name();
    static uint32_t        aton(const std::string& ip);
    static std::string     ntoa(uint32_t ip);
};

}
}

