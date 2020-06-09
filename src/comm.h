#pragma once
#include <stdint.h>

enum //actually, epoll_ctl sucks! u will know```if i want to modify DEL_WRITE from READ&WRITE, i can't call DEL_WRITE only(that will delete READ too)!
{
    IO_OPT_ADD_READ = 1,
    IO_OPT_DEL_READ = 2,
    IO_OPT_ADD_WRITE = 4,
    IO_OPT_DEL_WRITE = 8,
    IO_OPT_CLR = 16
};

#define FILTER_TYPE_NONE 0
#define FILTER_TYPE_AES  1
#define FILTER_TYPE_RSA  2
#define FILTER_TYPE_RC4  3

// #if defined(__i386__)||defined(WIN32) || defined(__x86_64__)
// 
// #define XHTONS
// #define XHTONL
// #define XHTONLL
// 
// #else /* big end */
// inline uint16_t XHTONS(uint16_t i16)    { return((i16 << 8) | (i16 >> 8)); }
// inline uint32_t XHTONL(uint32_t i32)    { return((uint32_t(XHTONS(i32)) << 16) | XHTONS(i32>>16)); }
// inline uint64_t XHTONLL(uint64_t i64)   { return((uint64_t(XHTONL((uint32_t)i64)) << 32) |XHTONL((uint32_t(i64>>32)))); }
// #endif /* __i386__ */
// 
// #define XNTOHS XHTONS
// #define XNTOHL XHTONL
// #define XNTOHLL XHTONLL

//client use little-endian, server(TCP/IP) use big-endian
inline uint16_t XHTONS(uint16_t i16)    { return((i16 << 8) | (i16 >> 8)); }
inline uint32_t XHTONL(uint32_t i32)    { return((uint32_t(XHTONS(i32)) << 16) | XHTONS(i32>>16)); }
inline uint64_t XHTONLL(uint64_t i64)   { return((uint64_t(XHTONL((uint32_t)i64)) << 32) |XHTONL((uint32_t(i64>>32)))); }

#define XNTOHS XHTONS
#define XNTOHL XHTONL
#define XNTOHLL XHTONLL

#if defined __GNUC__
#define likely(x) __builtin_expect ((x), 1)
#define unlikely(x) __builtin_expect ((x), 0)
#else
#define likely(x) (x)
#define unlikely(x) (x)
#endif

