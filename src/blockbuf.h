#pragma once
#include "logger.h"

#include <stdlib.h> //linux need it to identify free function!!!
#include <string.h> //memcpy, memmov
#include <sys/socket.h>
#include <errno.h>

template<unsigned BlockSize>
struct Allocator_malloc_free
{
    enum{mBLOCKSIZE = BlockSize};

    static char* ordered_malloc(size_t block)
    { return (char*)::malloc(mBLOCKSIZE * block); }
    static void ordered_free(char* block)
    { ::free(block); }
};

template<unsigned BlockSize>
struct Allocator_new_delete
{
    enum{mBLOCKSIZE = BlockSize};

    static char* ordered_malloc(size_t block)
    { return new char[mBLOCKSIZE * block]; }
    static void ordered_free(char* block)
    { delete[] block;}
};

#ifdef _USE_NEW_DELETE_ALLOCATOR_
    typedef Allocator_new_delete<1 * 1024>  Allocator_Block_1k;
    typedef Allocator_new_delete<2 * 1024>  Allocator_Block_2k;
    typedef Allocator_new_delete<4 * 1024>  Allocator_Block_4k;
    typedef Allocator_new_delete<8 * 1024>  Allocator_Block_8k;
    typedef Allocator_new_delete<16 * 1024> Allocator_Block_16k;
    typedef Allocator_new_delete<32 * 1024> Allocator_Block_32k;
    typedef Allocator_new_delete<64 * 1024> Allocator_Block_64k;
    typedef Allocator_new_delete<128* 1024> Allocator_Block_128k;
    typedef Allocator_new_delete<512* 1024> Allocator_Block_512k;
#else
    typedef Allocator_malloc_free<1 * 1024> Allocator_Block_1k;
    typedef Allocator_malloc_free<2 * 1024> Allocator_Block_2k;
    typedef Allocator_malloc_free<4 * 1024> Allocator_Block_4k;
    typedef Allocator_malloc_free<8 * 1024> Allocator_Block_8k;
    typedef Allocator_malloc_free<16 * 1024> Allocator_Block_16k;
    typedef Allocator_malloc_free<32 * 1024> Allocator_Block_32k;
    typedef Allocator_malloc_free<64 * 1024> Allocator_Block_64k;
    typedef Allocator_malloc_free<128* 1024> Allocator_Block_128k;
    typedef Allocator_malloc_free<512* 1024> Allocator_Block_512k;
#endif

template<typename TAllocator = Allocator_Block_4k, unsigned MaxBlockNum = 2>
class BlockBuf
{
public:
    enum{ mMAXBLOCKNUM = MaxBlockNum};
    enum{ mPos = size_t(-1) };//4294967295L
public:
    BlockBuf(): m_size(0), m_blockNum(0), m_data(NULL){}
    ~BlockBuf(){ this->free(); }

public:
    inline char*    data()      { return m_data; }
    inline char*    tail()      { return m_data + m_size; }
    inline size_t   size()      { return m_size; }
    inline size_t   blocksize() { return TAllocator::mBLOCKSIZE; }
    inline size_t   blocknum()  { return m_blockNum; }
    inline size_t   capacity()  { return m_blockNum * TAllocator::mBLOCKSIZE; }
    inline size_t   freespace() { return capacity() - size(); }
    inline bool     empty()     { return m_size == 0; }
    inline void     setsize(size_t n)   { m_size = n < capacity() ? n : capacity(); }
    inline void     free()      { TAllocator::ordered_free(m_data); m_data = NULL; m_blockNum = 0; m_size = 0; }

    bool    reserve(size_t n);
    bool    replace(size_t pos, const char* rep, size_t n);
    bool    append(const char* data, size_t len);
    void    erase(size_t pos, size_t n);
    void    clear();

protected:
    bool increase_capacity(size_t increase_size);

protected:
    size_t m_size;
    size_t m_blockNum;
    char*  m_data;
};

template<typename TAllocator, unsigned MaxBlockNum>
bool BlockBuf<TAllocator,MaxBlockNum>::increase_capacity(size_t increate_size)
{
    if ( increate_size == 0 || increate_size <= freespace() )
        return true;

    increate_size -= freespace();
    size_t newBlockNum = m_blockNum;
    newBlockNum += increate_size / TAllocator::mBLOCKSIZE;
    if ( increate_size % TAllocator::mBLOCKSIZE > 0) //still need a block
        newBlockNum++;

    if (newBlockNum > mMAXBLOCKNUM)
    {
        //log(Error, "increase capacity failed!!! newBlockNum:%d mMAXBLOCKNUM:%d",newBlockNum, mMAXBLOCKNUM);
        return false;
    }

    char* newData = TAllocator::ordered_malloc(newBlockNum);
    if (newData == NULL)
        return false; //log here
    if ( !empty() )
    {
        //copy old data and free old block
        memcpy(newData, m_data, m_size);
        TAllocator::ordered_free(m_data);
    }
    m_data = newData;
    m_blockNum = newBlockNum;
    return true;
}

template<typename TAllocator, unsigned MaxBlockNum>
bool BlockBuf<TAllocator, MaxBlockNum>::replace(size_t pos, const char* rep, size_t n)
{
    if (pos >= size()) // out_of_range ?
        return append(rep, n);

    if (pos + n >= size()) // replace all beginning with position pos
    {
        m_size = pos;
        return append(rep, n);
    }
    if (n > 0)
        memmove(m_data + pos, rep, n);
    return true;
}

template<typename TAllocator, unsigned MaxBlockNum>
bool BlockBuf<TAllocator, MaxBlockNum>::append(const char* data, size_t len)
{
    if ( len == 0 )
        return true; // no data

    if ( increase_capacity(len) )
    {
        memmove(tail(), data, len); // append
        m_size += len;
        return true;
    }
    else
    {
        //log(Error, "append failed!!! not enough buffer and increase failed",NULL);
        return false;
    }
}

template<typename TAllocator, unsigned MaxBlockNum>
bool BlockBuf<TAllocator, MaxBlockNum>::reserve(size_t n)
{
    return (n <= capacity() || increase_capacity(n - capacity()));
}

template<typename TAllocator, unsigned MaxBlockNum>
void BlockBuf<TAllocator, MaxBlockNum>::erase(size_t pos, size_t n)
{
    if (pos > size())
        pos = size();

    size_t m = size() - pos; // can erase
    if (n >= m)
        m_size = pos; // all clear after pos
    else
    {
        m_size -= n;
        memmove(m_data + pos, m_data + pos + n, m - n);
    }
}

template<typename TAllocator, unsigned MaxBlockNum>
void BlockBuf<TAllocator, MaxBlockNum>::clear()
{
    m_size = 0;
}

typedef BlockBuf<Allocator_Block_8k, 8>     Buffer8x8k;  //64k
typedef BlockBuf<Allocator_Block_8k, 16>    Buffer8x16k; //128k
typedef BlockBuf<Allocator_Block_8k, 32>    Buffer8x32k; //256k
typedef BlockBuf<Allocator_Block_32k, 16>   Buffer32x16k; //512k
typedef BlockBuf<Allocator_Block_32k, 32>   Buffer32x32k; //1M Wow!!!
typedef BlockBuf<Allocator_Block_64k, 64>   Buffer64x64k; //4M Wow!!!
typedef BlockBuf<Allocator_Block_128k, 64>  Buffer128x64k; //8M Holy!!!

