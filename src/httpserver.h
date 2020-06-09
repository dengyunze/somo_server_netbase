#pragma once

#include "tcpserver.h"

#include <stdint.h>
#include <stdlib.h>
#include <vector>
#include <string>
#include <map>

struct http_parser;
struct http_parser_settings;
struct ISNTcpHandler;
class TcpLink;
class HttpPeerLink;
class HttpRequest;
class HttpResponse;

/**
 * HttpPeerLink->Client.
 * 
 * handles the request from http client, and callback to client with ISNHttpServerHandler;
 * 
 * It's a simple http session, with no KEEP-ALIVE support.
 * 
 */
class HttpPeerLink {
public:
    HttpPeerLink(ISNTcpLink* link);
    ~HttpPeerLink();

public:
    void    set_handler(ISNHttpServerHandler* handler);
    int     on_data(const char* data, int len);
    void    close();

private:
    static int     on_begin(http_parser* parser);
    static int     on_url(http_parser*, const char *at, size_t length);
    static int     on_header_field(http_parser*, const char *at, size_t length);
    static int     on_header_value(http_parser*, const char *at, size_t length);
    static int     on_header_complete(http_parser*);
    static int     on_body(http_parser*, const char *at, size_t length);
    static int     on_complete(http_parser*);

private:
    std::string     format_headers();
    void    write_response(const std::string& res);

private:
    ISNTcpLink* m_pLink;
    ISNHttpServerHandler*   m_pHttpHandler;
    http_parser*            m_pHttParser;
    http_parser_settings*   m_pSettings;

    HttpRequest*    m_pRequest;
    std::string     m_strField;
    std::string     m_strValue;
};

class HttpRequest {
public:
    HttpRequest();
    ~HttpRequest();

public:
    std::string     get_method();
    void    set_method(const std::string& method);
    std::string     get_url();
    void    set_url(const std::string& url);
    std::map<std::string, std::string>  get_headers();
    void    add_header(const std::string& field, const std::string& value);
    std::string     get_body();
    void    set_body(const std::string& body);
    void    clear();

    std::string     format_headers();

private:
    std::string     m_strMethod;
    std::string     m_strUrl;
    std::map<std::string, std::string> m_mapHeaders;
    std::string     m_strBody;
};

/**
 * hold a http rsponse, include headers and body.
 * 
 */
struct HttpResponse {
public:
    HttpResponse();
    ~HttpResponse();

public:
    int     get_status();
    void    set_status(int status);
    std::string get_version();
    void    set_version(const std::string& version);
    std::string get_content_type();
    void    set_content_type(const std::string& content_type);
    std::string get_body();
    void    set_body(const std::string& body);

    std::string format();

private:
    int     m_nStatus;
    std::string m_strVersion;
    std::string m_strContentType;
    std::string m_strBody;
    
};

/**
 * simple http server with on "GET" and "POST" support, use by videoproxy to support query&start_rtmp_relay.
 * 
 * it's really a very simple server with libuv+http_parser, with no support on http/1.1 keepalive or http2, but the performance should be good!
 * 
 * [TODO] links with no data will be dropped after 120s.
 * 
 */
class HttpServer : public ISNHttpServer, public ISNLinkHandler
{
public:
    HttpServer();
    ~HttpServer();

public:
    virtual void    set_handler(ISNHttpServerHandler* handler);
    virtual void    listen(uint16_t port);
    virtual int     close();

public:
    virtual void on_connected(ISNLink* pLink);
    virtual void on_close(ISNLink* pLink);
    virtual int  on_data(const char* data, size_t len, ISNLink* pLink);
    
private:
    TcpServer*  m_pTcpServer;
    ISNHttpServerHandler*   m_pHttpHandler;
    std::map<uint32_t, HttpPeerLink*>   m_mapLink;
};

