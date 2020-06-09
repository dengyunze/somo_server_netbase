#include "linkfactory.h"
#include "isnet.h"
#include "tcpserver.h"
#include "udpserver.h"
#include "tcplink.h"
#include "udplink.h"
#include "timer.h"
#include "ioengine.h"

ISNTcpServer* SNFactory::createTcpServer() {
    return new TcpServer();
}


ISNUdpServer* SNFactory::createUdpServer() {
    return new UdpServer();
}

ISNTcpLink*   SNFactory::createTcpLink() {
    return new TcpLink();
}

ISNUdpLink*   SNFactory::createUdpLink() {
    return new UdpLink();
}

ISNTimer*     SNFactory::createTimer() {
    return new Timer();
}

static IOEngine* s_engine = NULL;
static bool s_engine_inited = false;
void SNStartup() {
    if( !s_engine ) {
        s_engine = new IOEngine();
    }

    if( !s_engine_inited ) {
        s_engine->init();
        s_engine_inited = true;
    }
}

void SNLoop() {
    if( !s_engine || !s_engine_inited ) {
        return;
    }
    s_engine->run();
}