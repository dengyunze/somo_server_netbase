#include "uni.h"
#include "comm.h"
#include <arpa/inet.h>
#include <sys/time.h>
#include <memory.h>
#include <netdb.h>
#include <string>
#include <sstream>
#include <unistd.h>

namespace uni
{
uint32_t sys_time_sec()
{
    return ::time(NULL);
}

uint64_t sys_time_msec()
{
    struct timeval _tv;
    ::gettimeofday(&_tv, NULL);
    return (_tv.tv_sec*1000) + (_tv.tv_usec/1000);
}

uint64_t random_64_num()
{
    struct timeval _tv;
    ::gettimeofday(&_tv, NULL);
    return _tv.tv_sec << 32 | (_tv.tv_usec << 12) | ( rand() & 0x00000FFF);
}

uint64_t assemble_id(uint32_t ip, uint16_t console_port)
{
    return (static_cast<uint64_t>(ip) << 16) | (static_cast<uint64_t>(console_port) & 0x000000000000FFFF);
}

void split_id(uint64_t id, uint32_t& out_ip, uint16_t& out_port)
{
    out_ip = static_cast<uint32_t>(id >> 32);
    out_port = static_cast<uint16_t>(id & 0x000000000000FFFF);
}

uint32_t peek_len(const void* data)
{
    if ( !data )
        return 0;

    uint32_t i32 = *((uint32_t*)data);
    i32 = XNTOHL(i32);
    return i32;
}

uint32_t get_host_ip(const char* hostname)
{
    struct hostent *host = gethostbyname(hostname);
    if (host == NULL)
    {
        return 0;
    }

    return *(u_long *)host->h_addr;
}

uint32_t addr_aton(const std::string& ip)
{
    return inet_addr( ip.c_str() );
}

std::string addr_ntoa(uint32_t ip)
{
    struct in_addr addr;
    memcpy(&addr, &ip, 4);
    return std::string(::inet_ntoa(addr));
}

void console_trim(std::string& stream)
{
    size_t i = 0;
    for ( ; i < 1024 && i < stream.size(); ++i )
    {
        if ( '\n' == stream[i] || '\r' == stream[i] )
            break;
    }
    stream = stream.substr(0,i);
}

void console_split(const std::string& stream, std::string& cmd, std::string& params, std::string pattern/* =" " */)
{
    size_t npos = stream.find_first_of(pattern);
    if(npos == std::string::npos)
    {
        cmd = stream;
    }
    else
    {
        cmd        = stream.substr(0, npos);
        params    = stream.substr(npos+1);
    }
}

void split(const std::string& stream, std::vector<std::string>& tokens, std::string pattern/* =" " */)
{
    std::string::size_type lastPos = stream.find_first_not_of(pattern, 0);
    std::string::size_type pos     = stream.find_first_of(pattern, lastPos);

    while ( std::string::npos != pos || std::string::npos != lastPos )
    {
        tokens.push_back(stream.substr(lastPos, pos - lastPos));
        lastPos = stream.find_first_not_of(pattern, pos);
        pos     = stream.find_first_of(pattern, lastPos);
    }
}

std::string TEST_RES(bool b)
{
    if (b)
        return " [PASS] ";
    else
        return " [FAIL] ";
}

std::string proto_to_stream(uint32_t uri, const Marshallable& m, uint16_t appid)
{
    Pack p(uri, appid);
    m.marshal(p);
    p.push_header();
    return std::string(p.header(), p.header_size() + p.body_size());
}

std::string print_32ids(const std::set<uint32_t>& ids, bool hexStyle/*=false*/)
{
    int idx = 0;
    std::ostringstream oss;
    if (hexStyle)
        oss << std::hex;
    oss << "[";
    for ( std::set<uint32_t>::const_iterator io = ids.begin(); io != ids.end(); ++io )
    {
        if (idx++ > 0)
            oss << ",";
        oss << *io;
    }
    oss << "]";
    return oss.str();
}

std::string print_64ids(const std::set<uint64_t>& ids, bool hexStyle/*=false*/)
{
    int idx = 0;
    std::ostringstream oss;
    if (hexStyle)
        oss << std::hex;
    oss << "[";
    for ( std::set<uint64_t>::const_iterator io = ids.begin(); io != ids.end(); ++io )
    {
        if (idx++ > 0)
            oss << ",";
        oss << *io;
    }
    oss << "]";
    return oss.str();
}

std::string print_mobiles(const std::set<std::string>& mobiles)
{
    int idx = 0;
    std::ostringstream oss;
    oss << "[";
    for ( std::set<std::string>::const_iterator io = mobiles.begin(); io != mobiles.end(); ++io )
    {
        if (idx++ > 0)
            oss << ",";
        oss << *io;
    }
    oss << "]";
    return oss.str();
}

std::string& trim(std::string &s) 
{
    if ( unlikely(s.empty()) ) 
    {
        return s;
    }

    s.erase(0,s.find_first_not_of(" "));
    s.erase(s.find_last_not_of(" ") + 1);
    return s;
}

std::string print_binary(const char* buf, size_t len)
{
    int output_len = len*8;  
    std::string output;  
    const char* m[] = {"0","1"};  
  
    for(int i = output_len - 1,j = 0; i >=0 ; --i,++j)  
    {  
        output.append(m[((uint8_t)buf[j/8] >> (i % 8)) & 0x01],1);  
    }  
    return output;
}

std::string get_host_name()
{
    char temp[255];
    if (gethostname(temp, 255)) {
        return "";
    }
    return temp;
}

uint32_t fnvhash(uint64_t v)
{
    return fnvhash((const char*)&v, sizeof(v));
}

uint32_t fnvhash(uint32_t v)
{
    return fnvhash((const char*)&v, sizeof(v));
}

uint32_t fnvhash(const char* pKey, size_t len)
{
    register uint32_t _prime = 16777619; //for 32 bit, 64 bit is 1099511628211
    register uint32_t _hash = 0x811C9DC5; //2166136261L for 32 bit, 64 bit is 14695981039346656037

    while(len--)
    {
        _hash = (_hash ^ (*(unsigned char*)pKey)) * _prime;
        pKey++;
    }

    _hash += _hash << 13;
    _hash ^= _hash >> 7;
    _hash += _hash << 3;
    _hash ^= _hash >> 17;
    _hash += _hash << 5;

    return _hash;
}

std::string int2string(int i)
{
    std::string str;
    std::stringstream ss;
    ss << i;
    ss >> str;
    return str;
}

std::string u642string(uint64_t i)
{
    std::string str;
    std::stringstream ss;
    ss << i;
    ss >> str;
    return str;
}

std::string base64_encode(const char* data, size_t len)
{
    const char EncodeTable[]="ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

    unsigned char _tmp[3] = {0};
    std::string _encode_res;
    uint32_t _num = len / 3;
    uint32_t _mod = len % 3;

    for ( uint32_t i = 1; i <= _num; ++i )
    {
        _tmp[0] = *data++;
        _tmp[1] = *data++;
        _tmp[2] = *data++;
        _encode_res += EncodeTable[_tmp[0] >> 2];
        _encode_res += EncodeTable[((_tmp[0] << 4) | (_tmp[1] >> 4)) & 0x3F];
        _encode_res += EncodeTable[((_tmp[1] << 2) | (_tmp[2] >> 6)) & 0x3F];
        _encode_res += EncodeTable[_tmp[2] & 0x3F];
    }
    switch (_mod)
    {
        case 1:
            {
                _tmp[0] = *data++;
                _encode_res += EncodeTable[(_tmp[0] & 0xFC) >> 2];
                _encode_res += EncodeTable[((_tmp[0] & 0x03) << 4)];
                _encode_res += "==";
                break;
            }
        case 2:
            {
                _tmp[0] = *data++;
                _tmp[1] = *data++;
                _encode_res += EncodeTable[(_tmp[0] & 0xFC) >> 2];
                _encode_res += EncodeTable[((_tmp[0] & 0x03) << 4) | ((_tmp[1] & 0xF0) >> 4)];
                _encode_res += EncodeTable[((_tmp[1] & 0x0F) << 2)];
                _encode_res += "=";
                break;
            }
    }

    return _encode_res;
}

std::string base64_decode(const char* data, size_t len)
{
    if (len % 4 != 0)
        return "";

    char _tmp[4];
    std::string _decode_res;
    while(len > 0) 
    {
        _tmp[0] = base64_char_idx(data[0]);  
        _tmp[1] = base64_char_idx(data[1]);  
        _tmp[2] = base64_char_idx(data[2]);  
        _tmp[3] = base64_char_idx(data[3]);  

        _decode_res += (_tmp[0] << 2) | (_tmp[1] >> 4);  
        _decode_res += (_tmp[1] << 4) | (_tmp[2] >> 2);  
        _decode_res += (_tmp[2] << 6) | (_tmp[3]);  

        data += 4;  
        len -= 4;
    }
    return _decode_res;
}

uint8_t base64_char_idx(char c)
{
    if((c >= 'A') && (c <= 'Z'))  
    {   return c - 'A';  
    }else if((c >= 'a') && (c <= 'z'))  
    {   return c - 'a' + 26;  
    }else if((c >= '0') && (c <= '9'))  
    {   return c - '0' + 52;  
    }else if(c == '+')  
    {   return 62;  
    }else if(c == '/')  
    {   return 63;  
    }else if(c == '=')  
    {   return 0;  
    }  
    return 0; 
}
std::string base64_encode(const std::string& str)
{
    return base64_encode(str.c_str(), str.size());
}
std::string base64_decode(const std::string& str)
{
    return base64_decode(str.c_str(), str.size());
}

std::string unix_timestamp_to_string(uint32_t timestamp)
{
    time_t t = (time_t)timestamp;
    struct tm tm = *localtime((time_t *)&t);
    char timeCStr[100];
    strftime(timeCStr, sizeof(timeCStr), "%Y-%m-%d %H:%M:%S", &tm);
    return std::string(timeCStr);
}

}//namespace uni

