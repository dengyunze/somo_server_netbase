#include "packbuf.h"

void PackBuffer::reserve_header(size_t header_size)
{
    bb.setsize(header_size);
}

void PackBuffer::append(const char* data)
{
    append( data, strlen(data) );
}

void PackBuffer::append(const char* data, size_t size)
{
    if( bb.append(data, size) )
        return;
    throw PackError(std::string("append buffer overflow"));
}

void PackBuffer::replace(size_t pos, const char* rep, size_t n)
{
    if( bb.replace(pos, rep, n) )
        return;
    throw PackError("replace buffer overflow");
}

void PackBuffer::reserve(size_t n)
{
    if( bb.reserve(n) )
        return;
    throw PackError("reserve buffer overflow");
}

