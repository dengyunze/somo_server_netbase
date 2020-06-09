#pragma once
#include "blockbuf.h"
#include <stdexcept>

struct PackError : public std::runtime_error
{
    PackError(const std::string& w) : std::runtime_error(w) {}
};

struct UnpackError : public std::runtime_error
{
    UnpackError(const std::string& w) : std::runtime_error(w) {}
};

class PackBuffer
{
public:
    inline char*    data()    { return bb.data();}
    inline size_t    size()  { return bb.size(); }

    void reserve_header(size_t header_size);
    void append(const char* data);
    void append(const char* data, size_t size);
    void replace(size_t pos, const char* rep, size_t n);
    void reserve(size_t n);

private:
    BlockBuf<Allocator_Block_64k, 65536> bb;
};

