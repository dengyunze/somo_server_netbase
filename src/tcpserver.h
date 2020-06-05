#pragma once

#include "isnet.h"

#include <stdint.h>
#include <stdlib.h>
#include <map>

struct uv_tcp_s;
struct uv_handle_s;
struct uv_stream_s;
struct uv_buf_t;

struct ISNTcpHandler;
class TcpLink;
class TcpServer : public ISNTcpServer
{
public:
    TcpServer();
    ~TcpServer();

public:
    virtual void    set_handler(ISNLinkHandler* handler);
    virtual int     listen(uint16_t base_port);
    virtual int     close();
    virtual uint16_t port();
    ISNTcpLink* get_link(uint32_t id);

private:
    static void    on_accept(uv_stream_s* server, int status);
    
private:
    ISNLinkHandler*m_pHander;
    uv_tcp_s*   m_pTcp;
    uint16_t    m_nPort;
    bool        m_bListen;
    uint64_t    m_nClients;
    std::map<uint32_t, TcpLink*>    m_mapLinks;
};

