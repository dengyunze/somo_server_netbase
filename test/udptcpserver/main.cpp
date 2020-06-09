#include "isnet.h"

#include <stdlib.h>

#define __CLASS__ "ServerHandler"

class ServerHandler : public ISNLinkHandler {
public:
    ServerHandler() {

    }

public:
    virtual int  on_data(const char* data, size_t len, uint32_t ip, short port) {
        m_nRecvs++;
        if( m_nRecvs%1000 == 0 ) {
            NETLOG(, "udp server handler on data, len=%d", len);
        }

        return len;
    }

private:
    int     m_nRecvs;
};

int main(int argc, char* argv[]) {
    SNStartup();

    ServerHandler handler;

    ISNUdpServer* udp_server = SNFactory::createUdpServer();
    udp_server->set_handler(&handler);
    udp_server->listen(8000);

    ISNTcpServer* tcp_server = SNFactory::createTcpServer();
    tcp_server->set_handler(&handler);
    tcp_server->listen(8000);

    SNLoop();

    return 0;
}