#include "isnet.h"
#include "logger.h"

#include <stdlib.h>

#define __CLASS__ "ServerHandler"

class ServerHandler : public ISNLinkHandler {
public:
    ServerHandler() {

    }

public:
    virtual int  on_data(const char* data, size_t len, uint32_t ip, short port) {
        m_nRecvs++;
        if( m_nRecvs%100 == 0 ) {
            FUNLOG(Info, "udp server handler on data, len=%d", len);
        }
    }

private:
    int     m_nRecvs;
};

int main(int argc, char* argv[]) {
    ISNStartup();

    ServerHandler handler;

    ISNUdpServer* server = SNLinkFactory::createUdpServer();
    server->set_handler(&handler);
    server->listen(8000);

    ISNLoop();

    return 0;
}