#include "udpserver.h"
#include "udppeerlink.h"
#include "timer.h"
#include "memitem.h"
#include "mempool.h"

#include "uv.h"
#include "env.h"
#include "ioengine.h"
#include "logger.h"
#include "timeutil.h"

#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define __CLASS__ "UdpServer"

//link expire if no packet for 15 seconds
#define UDP_EXPIRE  1000*15

char*   UdpServer::s_pStaticBuf = NULL;
UdpServer::UdpServer()
: m_pHandler(NULL)
, m_nReads(0)
{
    m_pUdp = (uv_udp_t*)malloc(sizeof(uv_udp_t));
    uv_udp_init(uv_default_loop(), m_pUdp);
    m_pUdp->data = this;

    m_pTimer = new Timer();
}

UdpServer::~UdpServer()
{
    delete m_pTimer;
    
    if( m_pUdp ) {
        uv_close((uv_handle_t*)m_pUdp, NULL);
        m_pUdp = NULL;
    }
}

void UdpServer::set_handler(ISNLinkHandler* handler) {
    m_pHandler = handler;
}

int UdpServer::listen(uint16_t base_port)
{
    FUNLOG(Info, "udp server listen, port=%d", base_port);
    
    sockaddr_in addr;
    uv_ip4_addr("0.0.0.0", base_port, &addr);
    uv_udp_bind(m_pUdp, (const sockaddr*)&addr, UV_UDP_REUSEADDR);
    uv_udp_recv_start(m_pUdp, on_alloc, on_read);
}

int UdpServer::close()
{
    if( m_pUdp ) {
        uv_close((uv_handle_t*)m_pUdp, NULL);
        m_pUdp = NULL;
    }
}

void UdpServer::answer(const char* data, int len, uint32_t ip, short port) {
    if( !m_pUdp ) {
        return;
    }

    struct sockaddr_in addr;
    addr.sin_family = AF_INET;
    addr.sin_port = port;
    addr.sin_addr.s_addr = ip;

    MemItem* mem = MemPool::Ins()->get(len);
    if( mem == NULL ) {
        FUNLOG(Error, "udp server send faled, no memitem!!! len=%d", len );
        return;
    }
    mem->assign(data, len);

    uv_buf_t buf = uv_buf_init(const_cast<char*>(mem->data()), len);
    uv_udp_send_t* send = (uv_udp_send_t*)malloc(sizeof(uv_udp_send_t));
    send->data = mem;
    uv_udp_send(send, m_pUdp, &buf, 1, (const sockaddr*)&addr, on_send);
}

void UdpServer::on_timer(int id) {
    std::set<uint64_t> expires;

    uint64_t now = timeutil::sys_time_msec();
    for(auto it = m_mapLink.begin(); it!=m_mapLink.end(); it++ ) {
        if( now >= it->second->stamp() + UDP_EXPIRE ) {
            FUNLOG(Info, "peer link expire, node=%llu", it->first);
            delete it->second;
            expires.insert(it->first);
        }
    }

    for( auto it=m_mapLink.begin(); it!=m_mapLink.end(); it++ ) {
        m_mapLink.erase(it);
    }
}

void UdpServer::on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    //FUNLOG(Info, "udp server alloc, size=%d", suggested_size);
    if( s_pStaticBuf == NULL ) {
        s_pStaticBuf = (char*)malloc(64*1024);
    }
    buf->base = s_pStaticBuf;
    buf->len = 64*1024;
    //buf->base = (char*) malloc(suggested_size);
    //buf->len = suggested_size;
}

void UdpServer::on_read(uv_udp_t* handle, ssize_t nread, const uv_buf_t* buf, const struct sockaddr* addr, unsigned flags) {
    UdpServer* server = (UdpServer*)handle->data;
    if( server == NULL ) {
        FUNLOG(Error, "udp server read failed, server==NULL", NULL);
        return;
    }

    if( nread < 0 ) {
        //uv_close((uv_handle_t*))
        FUNLOG(Error, "udp server read=-1!!!", NULL);
        return;
    } else if( nread == 0 ) {
        if( addr == NULL ) {
            //FUNLOG(Error, "udp server read nothing!!!", NULL);
            return;
        } else {
            FUNLOG(Error, "udp server read empty packet!!!", NULL);
            return;
        }
        return;
    } else {
        server->m_nReads++;
        if( server->m_nReads%10000 == 0 || server->m_nReads<= 5 ) {
            FUNLOG(Info, "udp server read, len=%d, count=%u", nread, server->m_nReads);
        }
        //char* temp = buf->base;
        //FUNLOG(Error, "udp server read, x[0]=%x, x[1]=%x, x[2]=%x, x[3]=%x, x[4]=%x, x[5]=%x, x[6]=%x, x[7]=%x", 
        //            temp[0], temp[1], temp[2], temp[3], temp[4], temp[5], temp[6], temp[7]);

        struct sockaddr_in* addr_in = (sockaddr_in*)addr;
        if( server->m_pHandler != NULL ) {
            UdpPeerLink* link = server->add_node(addr_in->sin_addr.s_addr, addr_in->sin_port);
            server->m_pHandler->on_data(buf->base, nread, link);
        }

        //[yunzed] for test only
        //server->answer(buf->base, nread, addr_in->sin_addr.s_addr, addr_in->sin_port);
    }
    
    if( flags & UV_UDP_MMSG_CHUNK == UV_UDP_MMSG_CHUNK )  {

    } else {
        //if( buf != NULL && buf->base != NULL ) {
        //    free(buf->base);
        //}
    }
}

void    UdpServer::on_send(uv_udp_send_t* req, int status) {
    MemItem* mem = (MemItem*)req->data;
    if( mem == NULL ) {
        FUNLOG(Error, "udp link send failed!!! mem==NULL, status=%d", status);
    } else {
        MemPool::Ins()->free(mem);
    }

    //free(req->bufs);
}

uint64_t    UdpServer::create_node(uint32_t ip, uint16_t port) {
    uint64_t node = ip;
    node << 32;
    node += port;

    return node;
}

UdpPeerLink* UdpServer::add_node(uint32_t ip, uint16_t port) {
    if( ip == 0 || port == 0 ) {
        FUNLOG(Error, "udp server add node, ip==0 or port==0, ip/port=%u/%d", ip, port);
        return NULL;
    }

    uint64_t node = create_node(ip, port);
    std::map<uint64_t, UdpPeerLink*>::iterator it = m_mapLink.find(node);
    if( it != m_mapLink.end() ) {
        return it->second;
    }

    UdpPeerLink* link = new UdpPeerLink(this, ip, port);
    m_mapLink[node] = link;
    return link;
}


