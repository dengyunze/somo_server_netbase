#pragma once

#include "isnet.h"
#include <string>
#include <stdint.h>

#define UDP_READ_BUF_SIZE 1024*1024 //1MB!

struct uv_udp_s;
struct uv_udp_send_s;
struct uv_buf_t;
struct uv_handle_s;
struct ISNLinkHandler;
class UdpServer;
class UdpPeerLink : public ISNUdpLink
{
public:
    UdpPeerLink(UdpServer* server, uint32_t ip, uint16_t port);
    ~UdpPeerLink();

public:
    virtual void    set_handler(ISNLinkHandler* handler);
    virtual int     connect(const std::string& ip, uint16_t port);
    virtual int     send(const char* data, size_t len);
    virtual int     send(const char* data, size_t len, uint32_t ip, uint16_t port);
    virtual int     close();
    virtual bool    is_tcp() { return false; }
    virtual std::string ip_str() { return m_strIP; }
    virtual uint32_t ip() { return m_nIP; }
    virtual uint16_t port() { return m_nPort; }

public:
    void        set_stamp(uint64_t stamp) {m_nStamp = stamp;}
    uint64_t    stamp() { return m_nStamp; }

private:
    UdpServer*  m_pUdpServer;
    std::string m_strIP;
    uint32_t    m_nIP;
    uint16_t    m_nPort;
    uint64_t    m_nLastErrorLogStamp;
    uint64_t    m_nSends;
    uint64_t    m_nStamp;
};

