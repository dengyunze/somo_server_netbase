#pragma once

#include "isnet.h"
#include <stdint.h>
#include <stdlib.h>
#include <string>
#include <deque>

struct uv_tcp_s;
struct uv_timer_s;
struct uv_connect_s;
struct uv_handle_s;
struct uv_stream_s;
struct uv_write_s;
struct uv_buf_t;

struct ILink;
struct ILinkHandler;
class TcpLink : public ISNTcpLink
{
public:
    TcpLink();
    TcpLink(uv_tcp_s* tcp);
    ~TcpLink();

public:
    virtual void    set_handler(ISNLinkHandler* handler);
    virtual int     connect(const std::string& ip, uint16_t port);
    virtual int     send(const char* data, size_t len);
    virtual int     close();
    virtual bool    is_tcp() { return true; }
    virtual bool    is_connected() { return m_bConnected; }
    virtual std::string ip_str() { return m_strIP; }
    virtual uint32_t ip() { return m_nIP; }
    virtual uint16_t port() { return m_bPort; }

public:
    uint32_t        id() { return m_nId; }
    ISNLinkHandler* link_handler() { return m_pHandler; }

private:
    static void    on_connect(uv_connect_s* req, int status);
    static void    on_alloc(uv_handle_s* handle, size_t suggested_size, uv_buf_t* buf);
    static void    on_read(uv_stream_s* stream, ssize_t nread, const uv_buf_t* buf);
    static void    on_write(uv_write_s* req, int status);
    static void    on_timer(uv_timer_s* handle);
    
private:
    uint32_t    m_nId;
    uv_tcp_s*   m_pTcp;
    uv_timer_s* m_pTimer;
    std::string m_strIP;
    uint32_t    m_nIP;
    uint16_t    m_bPort;
    bool        m_bPeerLink;

    ISNLinkHandler*m_pHandler;

    //status
    bool        m_bConnected;
    bool        m_bConnecting;
   

    //stats:
    uint32_t    m_nReconnects;
    uint32_t    m_nReconnectErrors;
    uint32_t    m_nSendErrors;
    uint32_t    m_nReads;
};

