#pragma once

#include "isnet.h"

#include <string>
#include <stdint.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define UDP_READ_BUF_SIZE 1024*1024 //1MB!

struct uv_udp_s;
struct uv_udp_send_s;
struct uv_buf_t;
struct uv_handle_s;
struct IUdpHandler;
class UdpLink : public ISNUdpLink
{
public:
    UdpLink();
    ~UdpLink();

public:
    virtual void    set_handler(ISNLinkHandler* handler);
    virtual int     connect(const std::string& ip, uint16_t port);
    virtual int     send(const char* data, size_t len);
    virtual int     send(const char* data, size_t len, uint32_t ip, uint16_t port);
    virtual int     close();
    virtual bool    is_tcp() { return false; }
    virtual std::string ip_str() { return m_strIP; }
    virtual uint32_t ip() { return m_nIP; }
    virtual uint16_t port() { return m_bPort; }

private:
    static void    on_alloc(uv_handle_s* handle, size_t suggested_size, uv_buf_t* buf);
    static void    on_send(uv_udp_send_s* req, int status);
    static void    on_read(uv_udp_s* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);

private:
    uv_udp_s*   m_pUdp;
    std::string m_strIP;
    uint32_t    m_nIP;
    uint16_t    m_bPort;
    ISNLinkHandler*m_pHandler;
    struct sockaddr_in m_peer_addr;
    char        m_readBuf[UDP_READ_BUF_SIZE];
    uint64_t    m_nLastErrorLogStamp;
    uint64_t    m_nSends;
    uint64_t    m_nReads;
    static char* s_pStaticBuf;
};

