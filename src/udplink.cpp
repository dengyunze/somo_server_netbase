#include "udplink.h"
#include "linkidallocator.h"
#include "mempool.h"
#include "memitem.h"

#include "uv.h"
#include "env.h"
#include "ioengine.h"
#include "logger.h"
#include "netaddr.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define __CLASS__ "UdpLink"

char*   UdpLink::s_pStaticBuf = NULL;
UdpLink::UdpLink()
: m_nLastErrorLogStamp(0)
, m_pUdp(NULL)
, m_pHandler(NULL)
, m_nSends(0)
, m_nReads(0)
{
    m_nId = LinkidAllocator::next();
}

UdpLink::~UdpLink()
{
    close();
}

void    UdpLink::set_handler(ISNLinkHandler* handler) {
    m_pHandler = handler;
}

int UdpLink::connect(const std::string& ip, uint16_t port)
{
    FUNLOG(Info, "udp link connect, ip=%s, port=%d", ip.c_str(), port);
    m_strIP = ip;
    m_nIP = netaddr::aton(ip.c_str());
    m_bPort = port;

    m_peer_addr.sin_family = AF_INET;
    m_peer_addr.sin_port = htons(port);
    m_peer_addr.sin_addr.s_addr = htonl(m_nIP);
    
    m_pUdp = (uv_udp_t*)malloc(sizeof(uv_udp_t));
    uv_udp_init(uv_default_loop(), m_pUdp);
    m_pUdp->data = this;
    
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    uv_udp_connect(m_pUdp, (const sockaddr*)&addr);
    uv_udp_recv_start(m_pUdp, on_alloc, on_read);
    
    return 0;
}

int  UdpLink::send(const char* data, size_t len) {
    if( !m_pUdp ) {
        return -1;
    }

    /*
    const char* temp = data;
    FUNLOG(Error, "udp link send, x[0]=%x, x[1]=%x, x[2]=%x, x[3]=%x, x[4]=%x, x[5]=%x, x[6]=%x, x[7]=%x", 
               temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7]);
    */

    m_nSends++;
    if( m_nSends%10000 == 0 || m_nSends<=5 ) {
        FUNLOG(Info, "udp link send, len=%d, sends=%llu", len, m_nSends);
    }

    //char* new_buf = (char*)malloc(len);
    //memcpy(new_buf, (const void*)data, len);
    MemItem* mem = MemPool::Ins()->get(len);
    if( mem == NULL ) {
        FUNLOG(Error, "udp link send faled, no memitem!!! len=%d", len );
        return 0;
    }
    mem->assign(data, len);

    uv_buf_t buf = uv_buf_init(const_cast<char*>(mem->data()), len);
    uv_udp_send_t* send = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    send->data = mem;
    uv_udp_send(send, m_pUdp, &buf, 1, NULL, on_send);

    return 0;
}

int UdpLink::send(const char* data, size_t len, uint32_t ip, uint16_t port) {
    if( !m_pUdp ) {
        return -1;
    }

    m_nSends++;
    if( m_nSends%10000 == 0 || m_nSends<=300 ) {
        FUNLOG(Info, "udp link send, len=%d, sends=%llu", len, m_nSends);
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = htonl(ip);

    MemItem* mem = MemPool::Ins()->get(len);
    if( mem == NULL ) {
        FUNLOG(Error, "udp link send faled, no memitem!!! len=%d", len );
        return 0;
    }
    mem->assign(data, len);

    uv_buf_t buf = uv_buf_init(const_cast<char*>(mem->data()), len);
    uv_udp_send_t* send = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    send->data = this;
    uv_udp_send(send, m_pUdp, &buf, 1, (const sockaddr*)&addr, on_send);

    return 0;
}

int UdpLink::close()
{
    if( m_pHandler ) {
        m_pHandler->on_close(this);
    }
    if( m_pUdp ) {
        uv_close((uv_handle_t*)m_pUdp, NULL);
        m_pUdp = NULL;
    }

    return 0;
}

void    UdpLink::on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    //buf->base = (char*) malloc(suggested_size);
    //buf->len = suggested_size;
    if( s_pStaticBuf == NULL ) {
        s_pStaticBuf = (char*)malloc(64*1024);
    }
    buf->base = s_pStaticBuf;
    buf->len = 64*1024;
}

void    UdpLink::on_send(uv_udp_send_t* req, int status) {
    MemItem* mem = (MemItem*)req->data;
    if( mem == NULL ) {
        FUNLOG(Error, "udp link send failed!!! mem==NULL, status=%d", status);
    } else {
        MemPool::Ins()->free(mem);
    }

    if( status <0 ) {
        FUNLOG(Error, "udp link send failed!!! status=%d", status);
    }

    free(req);
}

void    UdpLink::on_read(uv_udp_s* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
    UdpLink* link = (UdpLink*)handle->data;
    if( link == NULL ) {
        FUNLOG(Error, "udp link read failed, server==NULL", NULL);
        return;
    }

    if( nread > 0 ) {
        link->m_nReads++;
        if( link->m_nReads%10000 == 0 || link->m_nReads<= 5 ) {
            FUNLOG(Info, "udp link read, len=%d, count=%u", nread, link->m_nReads);
        }

        if( link->m_pHandler != NULL ) {
            struct sockaddr_in* addr_in = (sockaddr_in*)addr;
            link->m_pHandler->on_data(buf->base, nread, link);
        }
    } else {

    }
    
    //if( buf != NULL && buf->base != NULL ) {
    //    free(buf->base);
    //}
}