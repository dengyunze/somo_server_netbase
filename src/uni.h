#pragma once
#include <stdio.h>
#include <stdint.h>
#include <string>
#include <vector>
#include <set>
#include "packet.h"
#include "unpack.h"
#include "marshal.h"

namespace uni
{
    uint32_t        peek_len(const void* data);
    uint32_t        sys_time_sec();
    uint64_t        sys_time_msec();
    uint64_t        random_64_num();
    uint64_t        assemble_id(uint32_t ip, uint16_t console_port);
    void            split_id(uint64_t id, uint32_t& out_ip, uint16_t& out_port);
    uint32_t        get_host_ip(const char* hostname);
    uint32_t        addr_aton(const std::string& ip);
    std::string     addr_ntoa(uint32_t ip);
    void            console_trim(std::string& stream);
    void            console_split(const std::string& stream, std::string& cmd, std::string& params, std::string pattern=" ");
    void            split(const std::string& stream, std::vector<std::string>& tokens, std::string pattern=" ");
    std::string        TEST_RES(bool b);

    std::string        proto_to_stream(uint32_t uri, const Marshallable& m, uint16_t appid = 0);
    std::string        print_32ids(const std::set<uint32_t>& ids, bool hexStyle=false);
    std::string        print_64ids(const std::set<uint64_t>& ids, bool hexStyle=false);
    std::string        print_mobiles(const std::set<std::string>& mobiles);
    std::string&       trim(std::string &s);

    std::string        print_binary(const char* buf, size_t len);
    std::string        get_host_name();

    uint32_t        fnvhash(uint64_t v);
    uint32_t        fnvhash(uint32_t v);
    uint32_t        fnvhash(const char* pKey, size_t len);

    std::string        int2string(int i);
    std::string        u642string(uint64_t i);

    std::string        base64_encode(const std::string& str);
    std::string        base64_decode(const std::string& str);
    std::string        base64_encode(const char* data, size_t len);
    std::string        base64_decode(const char* data, size_t len);
    uint8_t            base64_char_idx(char c);

    std::string unix_timestamp_to_string(uint32_t timestamp);
}

