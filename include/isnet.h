#pragma once

#include <string>
#include <stdint.h>
#include <syslog.h>
#include <map>

struct ISNLink;
struct ISNLinkHandler;

struct ISNTcpServer;
struct ISNTcpLink;
struct ISNUdpServer;
struct ISNUdpLink;

/*
 * for easy log to syslog, refer to logger.h and logger.cpp
 */
extern void log(int level, const char *fmt, ...);
#define NETLOG(level, fmt, ...)   log(LOG_INFO, "[%s::%s]:" fmt,  __CLASS__, __FUNCTION__, __VA_ARGS__)



/**
 * link base class, it's usually a client link or server link object that mapped to a client link.
 *
 * ISNTcpLink, ISNUdpLink and ISNUdpPeerLink inherit from ISNLink.
 */
struct ISNLink {
    virtual ~ISNLink() {}
    virtual void    set_handler(ISNLinkHandler* handler) = 0;
    virtual int     connect(const std::string& ip, uint16_t port) = 0;
    virtual int     close() = 0;
    virtual bool    is_tcp() = 0;
    virtual uint32_t linkid() = 0;
    virtual int     send(const char* data, size_t len) = 0;
    virtual std::string ip_str() = 0;
    virtual uint32_t ip() = 0;
    virtual uint16_t port() = 0;
};

/**
 * server base class.

 * ISNUdpServer and ISNTcpServer inhereit from this.
 */
struct ISNServer {
public:
    virtual ~ISNServer() {}
    virtual void    set_handler(ISNLinkHandler* handler) = 0;
    virtual int     listen(uint16_t base_port) = 0;
    virtual int     close() = 0;
};

/**
 * link handler interface, callback to app when data come, link connected or link closed.
 *
 * Both server&client use this link handler to handle link events.
 */
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

/**
 * timer object, runs in epoll loop.
 */
class ISNTimer {
public:
    virtual ~ISNTimer() {}
    virtual void    init(int id) = 0;
    virtual void    set_handler(ISNTimerHandler* handler) = 0;
    virtual void    start(int interval) = 0;
    virtual void    stop() = 0;
    virtual void    close() = 0;
};

struct ISNHttpServerHandler {
public:
    virtual void    on_get(const std::string& url, const std::map<std::string, std::string>& headers, std::string& res) = 0;
    virtual void    on_post(const std::string& url, const std::map<std::string, std::string>& headers, const std::string& body, std::string& res) = 0;
};

struct ISNHttpServer {
    virtual ~ISNHttpServer() {}
    virtual void    set_handler(ISNHttpServerHandler* handler) = 0;
    virtual void    listen(uint16_t base_port) = 0;
    virtual int     close() = 0;
};

/**
 * somo net factory object
 */
class SNFactory {
public:
    static ISNTcpServer* createTcpServer();
    static ISNUdpServer* createUdpServer();
    static ISNHttpServer*createHttpServer();
    static ISNTcpLink*   createTcpLink();
    static ISNUdpLink*   createUdpLink();
    static ISNTimer*     createTimer();
};

/**
 * somo net enronment prepare! call this before everything happens.
 */
void SNStartup();

/**
 * somo net loop start!
 */
void SNLoop();
