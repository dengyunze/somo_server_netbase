#include "httpserver.h"
#include "tcpserver.h"
#include "httpparser.h"

#include "format.h"
#include "logger.h"
#include <string.h>

#define __CLASS__ "HttpServer"

HttpPeerLink::HttpPeerLink(ISNTcpLink* link)
: m_pHttpHandler(NULL)
, m_pLink(link)
{
    m_pSettings = (http_parser_settings*)malloc(sizeof(http_parser_settings));
    sn_http_parser_settings_init(m_pSettings);
    //m_pSettings->on_message_begin = (http_cb)on_begin;
    m_pSettings->on_url = (http_data_cb)&HttpPeerLink::on_url;
    m_pSettings->on_header_field = (http_data_cb)&HttpPeerLink::on_header_field;
    m_pSettings->on_header_value = (http_data_cb)&HttpPeerLink::on_header_value;
    m_pSettings->on_headers_complete = (http_cb)&HttpPeerLink::on_header_complete;
    m_pSettings->on_body = (http_data_cb)&HttpPeerLink::on_body;
    m_pSettings->on_message_complete = (http_cb)&HttpPeerLink::on_complete;

    m_pHttParser = (http_parser*)malloc(sizeof(http_parser));
    m_pHttParser->data = this;

    m_pRequest = new HttpRequest();
}

HttpPeerLink::~HttpPeerLink() {
    if( m_pHttParser ) {
        free(m_pHttParser);
    }
    if( m_pSettings ) {
        free(m_pSettings);
    }
    if( m_pRequest ) {
        delete m_pRequest;
    }
}

void    HttpPeerLink::set_handler(ISNHttpServerHandler* handler) {
    m_pHttpHandler = handler;
}

int     HttpPeerLink::on_data(const char* data, int len) {
    sn_http_parser_init(m_pHttParser, HTTP_REQUEST);
    sn_http_parser_execute(m_pHttParser, m_pSettings, data, len);

    return len;
}

void    HttpPeerLink::close() {
    m_pLink->close();
}

int     HttpPeerLink::on_begin(http_parser* parser) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    link->m_pRequest->clear();
}

int     HttpPeerLink::on_url(http_parser* parser, const char *data, size_t length) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    //link->m_strUrl.assign(data, length);
    link->m_pRequest->set_url( std::string(data, length) );

    switch(parser->method) {
        case HTTP_GET:
            //link->m_strMethod = "GET";
            link->m_pRequest->set_method( "GET" );
        break;
        case HTTP_POST:
            //link->m_strMethod = "POST";
            link->m_pRequest->set_method( "POST" );
        break;
    }

    log(Info, "http request url, url=%s", link->m_pRequest->get_url().c_str());
}

int     HttpPeerLink::on_header_field(http_parser* parser, const char *data, size_t length) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    link->m_strField.assign(data, length);

    log(Info, "http request header, field=%s", link->m_strField.c_str());
}

int     HttpPeerLink::on_header_value(http_parser* parser, const char *data, size_t length) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    link->m_strValue.assign(data, length);

    //link->m_mapHeaders[link->m_strField] = link->m_strValue;
    link->m_pRequest->add_header( link->m_strField, link->m_strValue);

    log(Info, "http request header, value=%s", link->m_strValue.c_str());
}

int     HttpPeerLink::on_header_complete(http_parser* parser) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    if( strncmp(link->m_pRequest->get_method().c_str(), "GET", link->m_pRequest->get_method().size()) == 0 ) {
        //GET has no body:
        return 1;
    }
    return 0;
}

int     HttpPeerLink::on_body(http_parser* parser, const char *data, size_t length) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    link->m_pRequest->set_body( std::string(data, length) );
}

int     HttpPeerLink::on_complete(http_parser* parser) {
    HttpPeerLink* link = (HttpPeerLink*)parser->data;
    log(Info, "http request complete, method=%s, url=%s, headers=%s, body=%s", link->m_pRequest->get_method().c_str(), link->m_pRequest->get_url().c_str(), link->format_headers().c_str(), link->m_pRequest->get_body().c_str());

    if( link->m_pHttpHandler != NULL ) {
        if( strncmp(link->m_pRequest->get_method().c_str(), "GET", link->m_pRequest->get_method().size())== 0 ) {
            std::string res;
            link->m_pHttpHandler->on_get(link->m_pRequest->get_url(), link->m_pRequest->get_headers(), res);

            HttpResponse response;
            response.set_body(res);

            std::string reshttp = response.format();
            link->m_pLink->send(reshttp.c_str(), reshttp.size());
            link->m_pLink->close();
        } else if( strncmp(link->m_pRequest->get_method().c_str(), "POST", link->m_pRequest->get_method().size())== 0 ) {
            std::string res;
            link->m_pHttpHandler->on_post(link->m_pRequest->get_url(), link->m_pRequest->get_headers(), link->m_pRequest->get_body(), res);

            HttpResponse response;
            response.set_body(res);

            std::string reshttp = response.format();
            link->m_pLink->send(reshttp.c_str(), reshttp.size());
            link->m_pLink->close();
        }
    }
}

std::string     HttpPeerLink::format_headers() {
    return m_pRequest->format_headers();
}

void    HttpPeerLink::write_response(const std::string& res) {

}


HttpRequest::HttpRequest() {

}

HttpRequest::~HttpRequest() {

}

std::string     HttpRequest::get_method() {
    return m_strMethod;
}

void    HttpRequest::set_method(const std::string& method) {
    m_strMethod = method;
}

std::string     HttpRequest::get_url() {
    return m_strUrl;
}

void    HttpRequest::set_url(const std::string& url) {
    m_strUrl = url;
}

std::map<std::string, std::string>  HttpRequest::get_headers() {
    return m_mapHeaders;
}

void    HttpRequest::add_header(const std::string& field, const std::string& value){
    m_mapHeaders[field] = value;
}

std::string     HttpRequest::get_body() {
    return m_strBody;
}

void    HttpRequest::set_body(const std::string& body) {
    m_strBody = body;
}

void    HttpRequest::clear() {
    m_strUrl = "";
    m_strMethod = "";
    m_mapHeaders.clear();
    m_strBody = "";
}

std::string     HttpRequest::format_headers() {
    std::string data;

    for( auto it=m_mapHeaders.begin(); it!=m_mapHeaders.end(); it++ ) {
        data += it->first + ":" + it->second + ";";
    }

    return data;
}


HttpResponse::HttpResponse() 
: m_nStatus(200)
{
    m_strVersion = "HTTP/1.1";
    m_strContentType = "application/json";
}

HttpResponse::~HttpResponse() {

}

int     HttpResponse::get_status() {
    return m_nStatus;
}

void    HttpResponse::set_status(int status) {
    m_nStatus = status;
}

std::string HttpResponse::get_version () {
    return m_strVersion;
}

void    HttpResponse::set_version(const std::string& version) {
    m_strVersion = version;
}

std::string HttpResponse::get_content_type() {
    return m_strContentType;
}

void    HttpResponse::set_content_type(const std::string& content_type) {
    m_strContentType = content_type;
}

std::string HttpResponse::get_body() {
    return m_strBody;
}

void    HttpResponse::set_body(const std::string& body) {
    m_strBody = body;
}

std::string HttpResponse::format() {
    std::string format = "";
    format += m_strVersion + " " + format::uint2str32(m_nStatus) + " OK\r\n";
    format += "Server:SOMO Video Proxy\r\n";
    format += "Content-Type:" + m_strContentType + "\r\n";
    //format += "Content-Length:" + format::uint2str32(m_strBody.size()) + "\r\n";
    format += "Connection: close\r\n\r\n";
    format += m_strBody;
    format += "\r\n\r\n";

    return format;
}



//HttpServer start here.
HttpServer::HttpServer() 
: m_pTcpServer(NULL)
{
    
}

HttpServer::~HttpServer() {
    if( m_pTcpServer ) {
        delete m_pTcpServer;
    }
    
}

void    HttpServer::set_handler(ISNHttpServerHandler* handler) {
    m_pHttpHandler = handler;
}

void    HttpServer::listen(uint16_t port) {
    close();

    m_pTcpServer = new TcpServer();
    m_pTcpServer->set_handler(this);
    m_pTcpServer->listen(port);
}

int     HttpServer::close() {
    if( m_pTcpServer ) {
        delete m_pTcpServer;
    }
}

void    HttpServer::on_connected(ISNLink* pLink) {
    auto it = m_mapLink.find(pLink->linkid());
    if( it == m_mapLink.end() ) {
        FUNLOG(Info, "http server connected, linkid=%d", pLink->linkid());
        HttpPeerLink* http_link = new HttpPeerLink((ISNTcpLink*)pLink);
        http_link->set_handler(m_pHttpHandler);
        m_mapLink[pLink->linkid()] = http_link;
    } else {
        FUNLOG(Error, "http server connected, duplicate linkid=%d", pLink->linkid());
    }
}

void    HttpServer::on_close(ISNLink* pLink) {
    uint32_t linkid = pLink->linkid();
    if( linkid <= 0 ) {
        FUNLOG(Error, "http server disconnected, invalid linkid=%d", pLink->linkid());
        return;
    }

    auto it = m_mapLink.find( pLink->linkid());
    if( it == m_mapLink.end() ) {
        FUNLOG(Error, "http server disconnected, link not exist, linkid=%d", pLink->linkid());
        return;
    }

    delete it->second;
    m_mapLink.erase(it);
}

int     HttpServer::on_data(const char* data, size_t len, ISNLink* pLink) {
    auto it = m_mapLink.find( pLink->linkid());
    if( it == m_mapLink.end() ) {
        FUNLOG(Error, "http server disconnected, link not exist, linkid=%d", pLink->linkid());
        return 0;
    }
    return it->second->on_data(data, len);
}

