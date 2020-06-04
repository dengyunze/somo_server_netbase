#include "isnet.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "tcplink.h"
#include "udplink.h"
#include "timer.h"
#include "ioengine.h"

ISNTcpServer* SNLinkFactory::createTcpServer() {
    return new TcpServer();
}


ISNUdpServer* SNLinkFactory::createUdpServer() {
    return new UdpServer();
}

ISNTcpLink*   SNLinkFactory::createTcpLink() {
    return new TcpLink();
}

ISNUdpLink*   SNLinkFactory::createUdpLink() {
    return new UdpLink();
}

ISNTimer*     SNLinkFactory::createTimer() {
    return new Timer();
}

static IOEngine* s_engine = NULL;
static bool s_engine_inited = false;
void ISNStartup() {
    if( !s_engine ) {
        s_engine = new IOEngine();
    }

    if( !s_engine_inited ) {
        s_engine->init();
        s_engine_inited = true;
    }
}

void ISNLoop() {
    if( !s_engine || !s_engine_inited ) {
        return;
    }
    s_engine->run();
}