#include "isnet.h"

#include <stdlib.h>
#include <string.h>

class ClientTimer : public ISNTimerHandler {
public:
    ClientTimer(ISNTcpLink* link) {
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

private:
    ISNTimer*     m_pTimer;
    ISNTcpLink*   m_pLink;
    char*         m_pBuf;
};


int main(int argc, char* argv[]) {
    SNStartup();

    ISNTcpLink* link = SNFactory::createTcpLink();
    link->connect( "127.0.0.1", 8000);

    ClientTimer timer(link);

    SNLoop();

    return 0;
}