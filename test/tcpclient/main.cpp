#include "isnet.h"

#include <stdlib.h>

class ClientTimer : public ISNTimerHandler {
public:
    ClientTimer(ISNTcpLink* link) {
        m_pLink = link;
        
        m_pTimer = SNFactory::createTimer();
        m_pTimer->init(1);
        m_pTimer->set_handler(this);
        m_pTimer->start(10);
    }

    ~ClientTimer() {
        m_pTimer->close();
    }

public:
    virtual void    on_timer(int id) {
        char* buf = new char[1200];
        m_pLink->send(buf, 1200);
    }

private:
    ISNTimer*     m_pTimer;
    ISNTcpLink*   m_pLink;
};


int main(int argc, char* argv[]) {
    SNStartup();

    ISNTcpLink* link = SNFactory::createTcpLink();
    link->connect( "127.0.0.1", 8000);

    ClientTimer timer(link);

    SNLoop();

    return 0;
}