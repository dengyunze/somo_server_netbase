#pragma once
#include "comm.h"
#include "packbuf.h"
#include <stdint.h>
#include <sstream>
#include <set>
#include <map>
#define HEADER_SIZE 10
#define COMMON_PACKET_SIZE 100


class Pack
{
public:
    virtual ~Pack() {}
    Pack() : _uri(0), _appid(0)            { _buf.reserve(COMMON_PACKET_SIZE); _buf.reserve_header(HEADER_SIZE); }
    Pack(uint32_t uri, uint16_t appid=0) : _uri(uri), _appid(appid)            { _buf.reserve(COMMON_PACKET_SIZE); _buf.reserve_header(HEADER_SIZE); }
    inline char* header()                { return _buf.data(); }
    inline char* body()                    { return _buf.data() + HEADER_SIZE; }
    inline size_t header_size()            { return HEADER_SIZE; }
    inline size_t body_size()            { return _buf.size() - header_size(); }

    inline Pack& push(const void * s, size_t n)        { _buf.append((const char *)s, n); return *this; }
    inline Pack& push(const void * s)                { _buf.append((const char *)s); return *this; }
    inline Pack& push_uint8(uint8_t u8)                { return push(&u8, 1); }
    inline Pack& push_uint16(uint16_t u16)            { u16 = XHTONS(u16); return push(&u16, 2); }
    inline Pack& push_uint32(uint32_t u32)            { u32 = XHTONL(u32); return push(&u32, 4); }
    inline Pack& push_uint64(uint64_t u64)            { u64 = XHTONLL(u64); return push(&u64, 8); }

    inline Pack& push_str32(const std::string& str)        { return push_str32(str.data(), str.size()); }
    inline Pack& push_str32(const void* s)            { return push_str32(s, strlen((const char *)s)); }
    inline Pack& push_str16(const std::string& str)        { return push_str16(str.data(), str.size()); }
    inline Pack& push_str16(const void* s)            { return push_str16(s, strlen((const char*)s)); }

    Pack& push_str16(const void* s, size_t len)
    {
        if (len > 0xFFFF)
            throw PackError("push_str16: string too long");
        return push_uint16(uint16_t(len)).push(s, len);
    }

    Pack& push_str32(const void* s, size_t len)
    {
        if (len > 0xFFFFFFFF)
            throw PackError("push_str32: string too long");
        return push_uint32(uint32_t(len)).push(s, len);
    }

    Pack& push_header()
    {
        uint32_t _lenght = XHTONL( header_size() + body_size() );
        uint32_t _xuri = XHTONL(_uri);
        uint16_t _xappid = XHTONS(_appid);
        _buf.replace(0, (const char*)&_lenght, 4);
        _buf.replace(4, (const char*)&_xuri, 4);
        _buf.replace(8, (const char*)&_xappid, 2);
        return *this;
    }

    Pack& reset(uint32_t uri, uint16_t appid)
    {
        _uri = uri;
        _appid = appid;
        _buf.reserve_header(HEADER_SIZE);
        return *this;
    }

protected:
    PackBuffer  _buf;
    uint32_t    _uri;
    uint16_t    _appid;
    
};

//--------------------------------------------  pack operator  ------------------------------------------------------------------
inline Pack& operator << (Pack& p, bool sign)                { p.push_uint8(sign ? 1 : 0); return p; }
inline Pack& operator << (Pack& p, uint8_t i8)                { p.push_uint8(i8); return p; }
inline Pack& operator << (Pack& p, uint16_t i16)            { p.push_uint16(i16); return p; }
inline Pack& operator << (Pack& p, uint32_t i32)            { p.push_uint32(i32); return p; }
inline Pack& operator << (Pack& p, uint64_t i64)            { p.push_uint64(i64); return p; }
inline Pack& operator << (Pack& p, int8_t i8)                { p.push_uint8((uint8_t)i8); return p; }
inline Pack& operator << (Pack& p, int16_t i16)                { p.push_uint16((uint16_t)i16); return p; }
inline Pack& operator << (Pack& p, int32_t i32)                { p.push_uint32((uint32_t)i32); return p; }
inline Pack& operator << (Pack& p, int64_t i64)                { p.push_uint64((uint64_t)i64); return p; }
inline Pack& operator << (Pack& p, const std::string& str)        { p.push_str16(str); return p; }

//------------------------------------------- container -------------------------------------------------------------------------
template <typename T1, typename T2>
inline Pack& operator << (Pack& p, const std::pair<T1, T2>& t)
{
    p << t.first << t.second;
    return p;
}

template <typename K, typename V>
inline Pack& operator << (Pack& p, const std::map<K,V>& item)
{
    marshal_container16(p, item);
    return p;
}

template <typename T>
inline Pack& operator << (Pack& p, const std::set<T>& c)
{
    marshal_container32(p, c);
    return p;
}

template <typename Tcontainer>
inline void marshal_container16(Pack& p, const Tcontainer& c)
{
    p.push_uint16( uint16_t(c.size()) );
    for ( typename Tcontainer::const_iterator io = c.begin(); io != c.end(); ++io )
        p << *io;
}

template <typename Tcontainer>
inline void marshal_container32(Pack& p, const Tcontainer& c)
{
    p.push_uint32( uint32_t(c.size()) );
    for ( typename Tcontainer::const_iterator io = c.begin(); io != c.end(); ++io )
        p << *io;
}

