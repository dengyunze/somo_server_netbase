#include "tcplink.h"
#include "mempool.h"
#include "memitem.h"
#include "linkidallocator.h"

#include "uv.h"
#include "env.h"
#include "logger.h"
#include "ioengine.h"
#include "timeutil.h"
#include "netaddr.h"

#include <string.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define __CLASS__ "TcpLink"

char*   TcpLink::s_pStaticBuf = NULL;

#define TCP_LINK_BUFFER_DEFAULT     1024*256
#define TCP_LINK_BUFFER_MAX         1024*1024

struct SendData {
    MemItem* mem;
    TcpLink* link;
};

TcpLink::TcpLink()
: m_pTcp(NULL)
, m_bPeerLink(false)
, m_bConnected(false)
, m_bConnecting(false)
, m_pHandler(NULL)
, m_nReconnects(0)
, m_nReads(0)
, m_nSends(0)
, m_nSendErrors(0)
{
    m_nCapacity = TCP_LINK_BUFFER_DEFAULT;
    m_pBuffer = new char[m_nCapacity];
    m_nPos = 0;
    m_nLen = 0;

    m_nId = LinkidAllocator::next();
    m_pTimer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
    m_pTimer->data = this;
    uv_timer_init(uv_default_loop(), m_pTimer);
}

TcpLink::TcpLink(uv_tcp_s* tcp) 
: m_pTcp(tcp)
, m_bPeerLink(true)
, m_bConnected(true)
, m_bConnecting(false)
, m_pHandler(NULL)
, m_nReconnects(0)
{
    m_nCapacity = TCP_LINK_BUFFER_DEFAULT;
    m_pBuffer = new char[m_nCapacity];
    m_nPos = 0;
    m_nLen = 0;

    m_nId = LinkidAllocator::next();
    m_pTimer = (uv_timer_t*)malloc(sizeof(uv_timer_t));
    m_pTimer->data = this;
    uv_timer_init(uv_default_loop(), m_pTimer);
    
    tcp->data = this;
    uv_read_start((uv_stream_t*)tcp, on_alloc, on_read);
}

TcpLink::~TcpLink()
{
    if( m_pTimer ) {
        uv_close((uv_handle_t*)m_pTimer, NULL);
        m_pTimer = NULL;
    }

    if( m_pTcp != NULL && !uv_is_closing((uv_handle_t*)m_pTcp) ) {
        uv_close((uv_handle_t*)m_pTcp, NULL);
        m_pTcp = NULL;
    }

    if( m_pBuffer ) {
        delete m_pBuffer;
    }
}

void TcpLink::set_handler(ISNLinkHandler* handler) {
    m_pHandler = handler;
}

int TcpLink::connect(const std::string& ip, uint16_t port) {
    m_strIP = ip;
    m_nIP = netaddr::aton(ip.c_str());
    m_bPort = port;

    if( m_nReconnects%15 == 0 ) {
        FUNLOG(Info, "tcp link reconnect!!! id=%u, ip=%s, port=%d, reconnects=%d", m_nId, ip.c_str(), port, m_nReconnects);
    }
    m_pTcp = (uv_tcp_t*)malloc(sizeof(uv_tcp_t));
    uv_tcp_init( uv_default_loop(), m_pTcp);
    m_pTcp->data = this;

    uv_connect_t* connect = (uv_connect_t*)malloc(sizeof(uv_connect_t));
    connect->data = this;

    //status:
    m_bConnecting = true;
    m_bConnected = false;

    //real connect:
    struct sockaddr_in addr;
    uv_ip4_addr(ip.c_str(), port, &addr);
    int r = uv_tcp_connect(connect, m_pTcp, (const sockaddr*)&addr, on_connect);
    if( r ) {
        FUNLOG(Error, "tcp link connect failed, id=%u, ip=%s, port=%d, error=%s", m_nId, ip.c_str(), port, uv_strerror(r));
        return -1;
    }

    //start a timer to check connected!!!
    uv_timer_start(m_pTimer, on_timer, 1000, 1000);

    return 0;
}

int TcpLink::send(const char* data, size_t len) {
    if( !m_pTcp ) {
        return -1;
    }
    if( m_bConnecting || !m_bConnected ) {
        m_nSendErrors++;
        if( m_nSendErrors%100 == 0 ) {
            FUNLOG(Error, "tcp link send failed, status!=connected! connecting=%d", m_bConnecting?1:0);
        }
        return -1;
    }

    MemItem* mem = MemPool::Ins()->get(len);
    if( mem == NULL ) {
        FUNLOG(Error, "udp link send faled, no memitem!!! len=%d", len );
        return 0;
    }
    mem->assign(data, len);

    uv_buf_t buf = uv_buf_init(const_cast<char*>(mem->data()), len);
    uv_write_t* write = (uv_write_t*)malloc(sizeof(uv_write_t));

    SendData* sendData = new SendData();
    sendData->mem = mem;
    sendData->link = this;
    write->data = sendData;
    int r = uv_write(write, (uv_stream_t*)m_pTcp, &buf, 1, on_write);
    if( r ) {
        FUNLOG(Error, "tcp link send failed, error=%s", uv_strerror(r));
        return -1;
    }

    sendData->link->m_nSends++;
    if( sendData->link->m_nSends%30000 == 0 || sendData->link->m_nSends<=5 ) {
        FUNLOG(Info, "tcp link send, sends=%d, len=%d", sendData->link->m_nSends, len);
    }

    return 0;
}

int TcpLink::close() {
    //no callback to on_close on use close!
    //if( m_pHandler ) {
    //    m_pHandler->on_close(this);
    //}
    m_bConnected = false;

    if( m_pTcp != NULL ) {
        uv_close((uv_handle_t*)m_pTcp, NULL);
        m_pTcp = NULL;
    }
    if( m_pTimer != NULL ) {
        uv_timer_stop(m_pTimer);
    }
    return 0;
}

void TcpLink::addData(const char* data, int len) {
    if( m_nPos + m_nLen + len >= m_nCapacity ) {
        if( m_nCapacity >= TCP_LINK_BUFFER_MAX ) {
            FUNLOG(Error, "tcp link add data failed! capacity too large, capacity=%d", m_nCapacity);
            m_nLen = 0;
            m_nPos = 0;
            return;
        } else {
            m_nCapacity = m_nPos + m_nLen + len;
            char* new_buf = new char[m_nCapacity];
            memcpy(new_buf, m_pBuffer+m_nPos, m_nLen);
            m_nPos = 0;

            delete m_pBuffer;
            m_pBuffer = new_buf;
        }
    }

    memcpy(m_pBuffer+m_nPos + m_nLen, data, len);
    m_nLen += len;
}

void TcpLink::removeData(int len) {
    m_nPos += len;
    m_nLen -= len;

    if( m_nPos >= m_nLen ) {
        //time to shrink:
        memcpy(m_pBuffer, m_pBuffer+m_nPos, m_nLen);
        m_nPos = 0;
    }
}

void TcpLink::closeError() {
    if( m_pHandler ) {
        m_pHandler->on_close(this);
    }
    m_bConnected = false;

    if( m_pTcp != NULL ) {
        uv_close((uv_handle_t*)m_pTcp, NULL);
        m_pTcp = NULL;
    }
    if( m_pTimer != NULL ) {
        uv_timer_stop(m_pTimer);
    }
}

void TcpLink::on_connect(uv_connect_t* req, int status) {
    TcpLink* link = (TcpLink*)req->data;
    if( link == NULL ) {
        FUNLOG(Error, "tcp link connected failed!!! link==NULL", NULL);
        return;
    }
    link->m_bConnecting = false;

    if( status < 0 ) {
        link->m_bConnected = false;
        link->m_nReconnectErrors++;
        if( link->m_nReconnectErrors%15 == 0 ) {
            FUNLOG(Error, "tcp link connected failed, error=%s", uv_strerror(status));
        }
        return;
    }
    link->m_bConnected = true;
    FUNLOG(Info, "tcp link connected!, id=%u, ip=%s, port=%d, status=%d", link->m_nId, link->m_strIP.c_str(), link->m_bPort, status);
    if( link->m_pHandler ) {
        link->m_pHandler->on_connected(link);
    }

    //stop the timer:
    uv_timer_stop(link->m_pTimer);

    int r = uv_read_start((uv_stream_t*)link->m_pTcp, on_alloc, on_read);
    if( r ) {
        FUNLOG(Error, "tcp link start read failed!!! error=%s", uv_strerror(r));
        return;
    }
}


void TcpLink::on_alloc(uv_handle_t* handle, size_t suggested_size, uv_buf_t* buf) {
    //buf->base = (char*) malloc(suggested_size);
    //buf->len = suggested_size;

    if( s_pStaticBuf == NULL ) {
        s_pStaticBuf = (char*)malloc(64*1024);
    }
    buf->base = s_pStaticBuf;
    buf->len = 64*1024;
}

void TcpLink::on_read(uv_stream_t* stream, ssize_t nread, const uv_buf_t* buf) {
    TcpLink* link = (TcpLink*)stream->data;
    if( link == NULL ) {
        FUNLOG(Error, "tcp link read failed, link==NULL", NULL);
        free(buf->base);
        return;
    }
    link->m_nReads++;
    if( link->m_nReads%30000 == 0 ) {
        FUNLOG(Info, "tcp link read, id=%d, len=%d, count=%u", link->m_nId, nread, link->m_nReads);
    }

    if (nread > 0) {
        link->addData(buf->base, nread);

        if( link->m_pHandler != NULL ) {
            int handled_len = link->m_pHandler->on_data(link->m_pBuffer, link->m_nLen, link);
            if( handled_len >0 ) {
                link->removeData(handled_len);
            }
        }

        //who free the buf?
        //free(buf->base);

        return;
     }
     if (nread < 0) {
        FUNLOG(Error, "tcp link read error %s", uv_err_name(nread));
        //link->close();

        //free buf:
        //free(buf->base);

        //start the reconnect timer:
        if( !link->m_bPeerLink ) {
            link->m_bConnected = false;
            uv_timer_start(link->m_pTimer, on_timer, 1000, 1000);
        } else {
            link->closeError();
        }
     }
}

void TcpLink::on_write(uv_write_t* req, int status) {
    SendData* data = (SendData*)req->data;
    if( data == NULL ) {
        FUNLOG(Error, "tcp link on write failed, SendData==NULL!!!, status=%d", status);
        return;
    }

    MemItem* mem = data->mem;
    if( mem == NULL ) {
        FUNLOG(Error, "tcp link on write failed!!! mem==NULL, status=%d", status);
    } else {
        MemPool::Ins()->free(mem);
    }

    TcpLink* link = data->link;
    if( link == NULL ) {
        FUNLOG(Error, "tcp link on write failed, link==NULL, status=%d", status);
    }

    if( status < 0 ) {
        FUNLOG(Error, "tcp write failed, error=%s", uv_strerror(status));

        if( link ) {
            link->close();
            //start the reconnect timer:
            if( !link->m_bPeerLink ) {
                uv_timer_start(link->m_pTimer, on_timer, 1000, 1000);
            }
        }
    }

    delete data;
    free(req);
}

void TcpLink::on_timer(uv_timer_t* handle) {
    //FUNLOG(Info, "tcp link timer!!!", NULL);
    TcpLink* link = (TcpLink*)handle->data;
    if( link == NULL ) {
        FUNLOG(Error, "tcp link timer failed, link==NULL", NULL);
        return;
    }

    if( !link->m_bConnected && !link->m_bConnecting ) {
        //FUNLOG(Info, "tcp link timer, reconnect now!!!", NULL);
        link->connect(link->m_strIP, link->m_bPort);
        link->m_nReconnects++;
    }
}