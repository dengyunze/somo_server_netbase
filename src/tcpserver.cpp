#include "tcpserver.h"
#include "tcplink.h"


#include "uv.h"
#include "env.h"
#include "comm.h"
#include "logger.h"
#include "ioengine.h"
#include "packbuf.h"
#include "uni.h"

#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define __CLASS__ "TcpServer"

TcpServer::TcpServer()
: m_pTcp(NULL)
, m_nPort(0)
, m_bListen(false)
, m_pHander(NULL)
, m_nClients(0)
{
    m_pTcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init( uv_default_loop(), m_pTcp);
    m_pTcp->data = this;
}

TcpServer::~TcpServer()
{
    uv_tcp_close_reset(m_pTcp, NULL);
    m_pTcp = NULL;
}

void    TcpServer::set_handler(ISNLinkHandler* handler) {
    m_pHander = handler;
}

int TcpServer::listen(uint16_t base_port) {
    FUNLOG(Info, "tcp server listen, port=%d", base_port);
    m_nPort = base_port;

    struct sockaddr_in   addr;
    uv_ip4_addr("0.0.0.0", base_port, &addr);
    uv_tcp_bind(m_pTcp, (const struct sockaddr*)&addr, 0);
    int r = uv_listen((uv_stream_t*)m_pTcp, 100, on_accept);
    if( r ) {
        FUNLOG(Error, "tcp server listen failed, error=%s", uv_strerror(r) );
    }
    m_bListen = true;
    
    return 0;
}

int TcpServer::close() {
    m_bListen = false;
    if( m_pTcp != NULL ) {
        uv_tcp_close_reset(m_pTcp, NULL);
        m_pTcp = NULL;
    }
    return 0;
}

uint16_t TcpServer::port() {
    return m_nPort;
}

ISNTcpLink*  TcpServer::get_link(uint32_t id) {
    auto it = m_mapLinks.find(id);
    if( it == m_mapLinks.end() ) {
        FUNLOG(Error, "tcp server get link failed!!! id=%u", id);
        return NULL;
    }        

    return it->second;
}

void TcpServer::on_accept(uv_stream_t* server, int status) {
    TcpServer* tcp = (TcpServer*)server->data;
    if( tcp == NULL ) {
        FUNLOG(Error, "tcp server accept, server==NULL", NULL);
        return;
    }
    tcp->m_nClients++;
    if( tcp->m_nClients%100 == 0 ) {
        FUNLOG(Info, "tcp server accept, clients=%llu, status=%d", tcp->m_nClients, status);
    }
    if( status < 0 ) {
        FUNLOG(Error, "tcp server accept error, %d", uv_strerror(status));
        return;
    }

    uv_tcp_t* client = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init(uv_default_loop(), client);

    //start accept:
    if( uv_accept(server, (uv_stream_t*)client) == 0 ) {
        //bind the uv_tcp_t to TcpLink!
        TcpLink* link = new TcpLink(client);
        link->set_handler(tcp->m_pHander);

        //add toe tcp server link map:
        tcp->m_mapLinks[link->id()] = link;
    } else {
        uv_close((uv_handle_t*)client, NULL);
    }
}

