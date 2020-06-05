#include "udppeerlink.h"
#include "udpserver.h"

#include "uv.h"
#include "env.h"
#include "comm.h"
#include "ioengine.h"
#include "logger.h"
#include "packbuf.h"
#include "uni.h"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>

#define __CLASS__ "UdpPeerLink"

UdpPeerLink::UdpPeerLink(UdpServer* server, uint32_t ip, uint16_t port)
: m_pUdpServer(server)
, m_nIP(ip)
, m_nPort(port)
, m_nLastErrorLogStamp(0)
, m_nSends(0)
, m_nStamp(0)
{
    m_strIP = uni::addr_ntoa(ip);
}

UdpPeerLink::~UdpPeerLink()
{
}

void    UdpPeerLink::set_handler(ISNLinkHandler* handler) {
    
}

int UdpPeerLink::connect(const std::string& ip, uint16_t port)
{
    return 0;
}

int  UdpPeerLink::send(const char* data, size_t len) {
    if( !m_pUdpServer ) {
        return -1;
    }

    m_nSends++;
    if( m_nSends%1000 == 0 || m_nSends<=5 ) {
        FUNLOG(Info, "udp peer link send, len=%d, sends=%llu", len, m_nSends);
    }
    m_pUdpServer->answer(data, len, m_nIP, m_nPort);

    return 0;
}

int UdpPeerLink::send(const char* data, size_t len, uint32_t ip, uint16_t port) {
    if( !m_pUdpServer ) {
        return -1;
    }

    m_nSends++;
    if( m_nSends%1000 == 0 || m_nSends<=5 ) {
        FUNLOG(Info, "udp peer link send, len=%d, sends=%llu", len, m_nSends);
    }

    m_pUdpServer->answer(data, len, ip, port);

    return 0;
}

int UdpPeerLink::close()
{
    return 0;
}
