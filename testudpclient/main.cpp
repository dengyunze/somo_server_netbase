#include "udplink.h"
#include "ilinkhandler.h"
#include "ioengine.h"
#include "timer.h"
#include "uni.h"
#include <stdlib.h>

#define __CLASS__ "ClientTimer"

class ClientTimer : public ISNTimerHandler, public ISNLinkHandler {
public:
    ClientTimer(ISNLink* link) {
        m_pLink = link;
        
        m_pTimer = new Timer();
        m_pTimer->init(1);
        m_pTimer->setHandler(this);
        m_pTimer->start(100);
    }

    ~ClientTimer() {
        m_pTimer->close();
    }

public:
    virtual void    onTimer(int id) {
        char* buf = new char[1200];
        memset(buf, 0, 1200);
        m_pLink->send(buf, 1200);
    }

    virtual int  on_data(const char* data, size_t len, uint32_t ip, short port) {
        FUNLOG(Info, "udp link on data, len=%u", len);
    }

private:
    ISNTimer*   m_pTimer;
    ISNLink*    m_pLink;
};


int main(int argc, char* argv[]) {
    ISNStartup();

    ISNLink* link = SNLinkFactory::createUdpLink();
    link->connect( "127.0.0.1", 8000);
    link->send("good", 4);

    ClientTimer timer(link);

    ISNLoop();

    return 0;
}