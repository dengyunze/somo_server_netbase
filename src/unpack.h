#pragma once
#include "comm.h"
#include "packbuf.h"
#include <stdint.h>
#include <sstream>
#include <set>
#include <map>

class Unpack
{
public:
    Unpack(const void* data, size_t size) : m_data((const char*)data), m_size(size) {}
    ~Unpack() {}

public:
    inline bool empty() const    { return m_size == 0; }
    const char* data() const    { return m_data; }
    inline size_t size() const    { return m_size; }
    inline size_t pkglen() const    { return _pkglen; }
    inline uint32_t uri() const     { return _uri; }
    inline uint16_t appid() const    { return _appid; }

    uint32_t peek_len() const
    {
        if(m_size < 4u)
        {
            std::ostringstream oss;
            oss << "peek_len: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
        }
        uint32_t i32 = *((uint32_t*)m_data);
        i32 = XNTOHL(i32);
        return i32;
    }

    uint8_t pop_uint8() const
    {
        if ( m_size < 1u )
        {
            std::ostringstream oss;
            oss << "pop_uint8: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
        }
        uint8_t i8 = *((uint8_t*)m_data);
        m_data += 1u; m_size -= 1u;
        return i8;
    }
    
    uint16_t pop_uint16() const
    {
        if ( m_size < 2u )
    {
        std::ostringstream oss;
        oss << "pop_uint16: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
    }

        uint16_t i16 = *((uint16_t*)m_data);
        i16 = XNTOHS(i16);

        m_data += 2u; m_size -= 2u;
        return i16;
    }

    uint32_t pop_uint32() const
    {
        if ( m_size < 4u )
    {
        std::ostringstream oss;
        oss << "pop_uint32: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
    }

        uint32_t i32 = *((uint32_t*)m_data);
        i32 = XNTOHL(i32);
        m_data += 4u; m_size -= 4u;
        return i32;
    }

    uint32_t peek_uint32() const {
        if (m_size < 4u)
            throw UnpackError("peek_uint32: not enough data");
        uint32_t i32 = *((uint32_t*)m_data);
        i32 = XNTOHL(i32);
        return i32;
    }
    uint64_t pop_uint64() const
    {
        if ( m_size < 8u )
    {
        std::ostringstream oss;
        oss << "pop_uint64: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
    }
        uint64_t i64 = *((uint64_t*)m_data);
        i64 = XNTOHLL(i64);
        m_data += 8u; m_size -= 8u;
        return i64;
    }

    std::string pop_str16() const
    {
        size_t _len = pop_uint16();
        if ( m_size < _len )
        {
            std::ostringstream oss;
            oss << "pop_str16: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
        }
        const char* p = m_data;
        m_data += _len;
        m_size -= _len;
        return std::string(p, _len);
    }

    void pop_str16(std::string& s) const
    {
        size_t _len = pop_uint16();
        if ( m_size < _len )
        {
            std::ostringstream oss;
            oss << "pop_str16: not enough data. uri:" << _uri<<", len:"<<_len;
            throw UnpackError(oss.str());
        }
        s.assign(m_data, _len);
        m_data += _len;
        m_size -= _len;
    }

    std::string pop_str32() const
    {
        size_t _len = pop_uint32();
        if ( m_size < _len )
        {
            std::ostringstream oss;
            oss << "pop_str32: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
        }
        const char* p = m_data;
        m_data += _len;
        m_size -= _len;
        return std::string(p, _len);
    }

    void pop_str32(std::string& s) const
    {
        size_t _len = pop_uint32();
        if ( m_size < _len )
        {
            std::ostringstream oss;
            oss << "pop_str32: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
        }
        s.assign(m_data, _len);
        m_data += _len;
        m_size -= _len;
    }

    std::string pop_packet() const
    {
        size_t _len = peek_len();
        if(m_size < _len)
        {
            std::ostringstream oss;
            oss << "pop_packet: not enough data. uri:" << _uri;
            throw UnpackError(oss.str());
        }
        const char* p = m_data;
        m_data += _len;
        m_size -= _len;
        return std::string(p,_len);
    }

    void pop_header()
    {
        _pkglen = pop_uint32();
        _uri = pop_uint32();
        _appid = pop_uint16();
    }

private:
    mutable const char* m_data;
    mutable size_t m_size;
    size_t    _pkglen;
    size_t    _uri;
    uint16_t _appid;
};

//------------------------------------------- unpack operator --------------------------------------------------------------------
inline const Unpack& operator >> (const Unpack& up, bool& sign)        { sign =  (up.pop_uint8() == 0) ? false : true; return up; }
inline const Unpack& operator >> (const Unpack& up, uint8_t& i8)    { i8 =  up.pop_uint8(); return up; }
inline const Unpack& operator >> (const Unpack& up, uint16_t& i16)    { i16 = up.pop_uint16(); return up; }
inline const Unpack& operator >> (const Unpack& up, uint32_t& i32)    { i32 = up.pop_uint32(); return up; }
inline const Unpack& operator >> (const Unpack& up, uint64_t& i64)    { i64 = up.pop_uint64(); return up; }
inline const Unpack& operator >> (const Unpack& up, int8_t& i8)        { i8 =  (int8_t)up.pop_uint8(); return up; }
inline const Unpack& operator >> (const Unpack& up, int16_t& i16)    { i16 = (int16_t)up.pop_uint16(); return up; }
inline const Unpack& operator >> (const Unpack& up, int32_t& i32)    { i32 = (int32_t)up.pop_uint32(); return up; }
inline const Unpack& operator >> (const Unpack& up, int64_t& i64)    { i64 = (int64_t)up.pop_uint64(); return up; }
inline const Unpack& operator >> (const Unpack& up, std::string& str)    { str = up.pop_str16(); return up; }


template <typename T1, typename T2>
inline const Unpack& operator >> (const Unpack& up, std::pair<const T1, T2>& t)
{
    const T1& const_first = t.first;
    T1& _first = const_cast<T1&>(const_first);
    up >> _first >> t.second;
    return up;
}

template <typename K, typename V>
inline const Unpack& operator >> (const Unpack& up, std::map<K,V>& item)
{
    unmarshal_container16(up, std::inserter(item, item.begin()));
    return up;
}


template <typename T1, typename T2>
inline const Unpack& operator >> (const Unpack& up, std::pair<T1, T2>& t)
{
    up >> t.first >> t.second;
    return up;
}

template <typename T>
inline const Unpack& operator >> (const Unpack& up, std::set<T>& t)
{
    unmarshal_container32(up, std::inserter(t, t.begin()));
    return up;
}

template <typename T>
inline const Unpack& operator >> (const Unpack& up, std::insert_iterator<T>& io)
{
    unmarshal_container32(up, io);
    return up;
}

template <typename OutputIterator>
inline void unmarshal_container16(const Unpack& up, OutputIterator io)
{
    for ( uint16_t idx = up.pop_uint16(); idx > 0; --idx )
    {
        typename OutputIterator::container_type::value_type tmp;
        up >> tmp;
        *io = tmp;
        ++io;
    }
}

template <typename OutputIterator>
inline void unmarshal_container32(const Unpack& up, OutputIterator io)
{
    for ( uint32_t idx = up.pop_uint32(); idx > 0; --idx )
    {
        typename OutputIterator::container_type::value_type tmp;
        up >> tmp;
        *io = tmp;
        ++io;
    }
}
