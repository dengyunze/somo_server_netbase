#include "isnet.h"
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define __CLASS__ "ClientTimer"

class ClientTimer : public ISNTimerHandler, public ISNLinkHandler {
public:
    ClientTimer(ISNLink* link) {
        m_pLink = link;
        
        m_pTimer = SNFactory::createTimer();
        m_pTimer->init(1);
        m_pTimer->set_handler(this);
        m_pTimer->start(1);

        m_pBuf = new char[1300];
        memset(m_pBuf, 0, 1300);
    }

    ~ClientTimer() {
        m_pTimer->close();
    }

public:
    virtual void    on_timer(int id) {
        for( int i=0; i<200; i++ ) {
            m_pLink->send(m_pBuf, 1300);
        }
    }

    virtual int  on_data(const char* data, size_t len, uint32_t ip, short port) {
        //NETLOG(Info, "udp link on data, len=%u", len);
    }

private:
    ISNTimer*   m_pTimer;
    ISNLink*    m_pLink;
    char*       m_pBuf;
};


int main(int argc, char* argv[]) {
    SNStartup();

    ISNLink* link = SNFactory::createUdpLink();
    link->connect( "127.0.0.1", 8000);
    link->send("good", 4);

    ClientTimer timer(link);

    SNLoop();

    return 0;
}