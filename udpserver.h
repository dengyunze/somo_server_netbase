#pragma once

#include "isnet.h"
#include <stdint.h>
#include <unistd.h>
#include <map>

struct uv_buf_t;
struct uv_handle_s;
struct uv_udp_send_s;
struct uv_udp_s;
struct ISNLinkHandler;
class UdpPeerLink;
class UdpServer : public ISNUdpServer, public ISNTimerHandler
{
public:
    UdpServer();
    ~UdpServer();

public:
    virtual void    set_handler(ISNLinkHandler* handler);
    virtual int     listen(uint16_t base_port);
    virtual int     close();
    virtual void    answer(const char* data, int len, uint32_t ip, short port);
    virtual void    on_timer(int id);

private:
    static void    on_alloc(uv_handle_s* handle, size_t suggested_size, uv_buf_t* buf);
    static void    on_read(uv_udp_s* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags);
    static void    on_send(uv_udp_send_s* req, int status);

private:
    uint64_t    create_node(uint32_t ip, uint16_t port);
    UdpPeerLink*add_node(uint32_t ip, uint16_t port);

private:
    uv_udp_s*   m_pUdp;
    ISNTimer*   m_pTimer;
    ISNLinkHandler*m_pHandler;
    std::map<uint64_t, UdpPeerLink*>    m_mapLink;
    uint64_t    m_nReads;
    static char* s_pStaticBuf;
};

