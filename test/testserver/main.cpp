#include "tcpserver.h"
#include "ioengine.h"
#include "ilinkhandler.h"
#include "logger.h"

#include <stdlib.h>

#define __CLASS__ "ServerHandler"

class ServerHandler : public ITcpHandler {
public:
    ServerHandler() {

    }

public:
    virtual int  on_data(const char* data, size_t len, ITcpLink* pLink) {
        m_nRecvs++;
        if( m_nRecvs%100 == 0 ) {
            FUNLOG(Info, "server handler on data, len=%d", len);
        }
    }

private:
    int     m_nRecvs;
};

int main(int argc, char* argv[]) {
    IOEngine engine;
    engine.init();

    ServerHandler handler;

    TcpServer server;
    server.set_handler(&handler);
    server.listen(8000);

    engine.run();

    return 0;
}