#pragma once

#include <string>
#include <stdint.h>

struct ISNLink;
struct ISNLinkHandler;

struct ISNTcpServer;
struct ISNTcpLink;
struct ISNUdpServer;
struct ISNUdpLink;

struct ISNLink {
    virtual ~ISNLink() {}
    virtual void    set_handler(ISNLinkHandler* handler) = 0;
    virtual int     connect(const std::string& ip, uint16_t port) = 0;
    virtual int     close() = 0;
    virtual bool    is_tcp() = 0;
    virtual int     send(const char* data, size_t len) = 0;
    virtual std::string ip_str() = 0;
    virtual uint32_t ip() = 0;
    virtual uint16_t port() = 0;
};

struct ISNServer {
public:
    virtual ~ISNServer() {}
    virtual void    set_handler(ISNLinkHandler* handler) = 0;
    virtual int     listen(uint16_t base_port) = 0;
    virtual int     close() = 0;
};

struct ISNLinkHandler
{
    virtual ~ISNLinkHandler() {}
    virtual void on_connected(ISNLink* pLink) {}
    virtual void on_close(ISNLink* pLink) {}
    virtual int  on_data(const char* data, size_t len, ISNLink* pLink) { return len; }
};

struct ISNTcpServer : public ISNServer {
public:
    virtual ~ISNTcpServer() {}
    virtual void    set_handler(ISNLinkHandler* handler) = 0;
    virtual int     listen(uint16_t base_port) = 0;
    virtual int     close() = 0;
    virtual uint16_t port() = 0;
    virtual ISNTcpLink* get_link(uint32_t id) = 0;
};

struct ISNTcpLink : public ISNLink
{
public:
    virtual bool    is_connected() = 0;

};

struct ISNUdpServer : public ISNServer {
public:
    virtual ~ISNUdpServer() {}
    virtual void    answer(const char* data, int len, uint32_t ip, short port) = 0;
};

struct ISNUdpLink : public ISNLink
{
public:
    virtual int     send(const char* data, size_t len, uint32_t ip, uint16_t port) = 0;
};

struct ISNUdpPeerLink : public ISNLink
{
public:
    virtual int     send(const char* data, size_t len, uint32_t ip, uint16_t port) = 0;
};


struct ISNTimerHandler {
    virtual ~ISNTimerHandler() {}
    virtual void    on_timer(int id) = 0;
};

class ISNTimer {
public:
    virtual ~ISNTimer() {}
    virtual void    init(int id) = 0;
    virtual void    set_handler(ISNTimerHandler* handler) = 0;
    virtual void    start(int interval) = 0;
    virtual void    stop() = 0;
    virtual void    close() = 0;
};


class SNLinkFactory {
public:
    static ISNTcpServer* createTcpServer();
    static ISNUdpServer* createUdpServer();
    static ISNTcpLink*   createTcpLink();
    static ISNUdpLink*   createUdpLink();
    static ISNTimer*     createTimer();
};

void ISNStartup();
void ISNLoop();
