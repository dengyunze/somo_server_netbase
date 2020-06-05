#include "tcplink.h"
#include "ioengine.h"
#include "timer.h"
#include <stdlib.h>

class ClientTimer : public ITimerHandler {
public:
    ClientTimer(TcpLink* link) {
        m_pLink = link;
        
        m_pTimer = new Timer();
        m_pTimer->init(1);
        m_pTimer->setHandler(this);
        m_pTimer->start(10);
    }

    ~ClientTimer() {
        m_pTimer->close();
    }

public:
    virtual void    onTimer(int id) {
        char* buf = new char[1200];
        m_pLink->send(buf, 1200);
    }

private:
    ITimer*     m_pTimer;
    TcpLink*    m_pLink;
};


int main(int argc, char* argv[]) {
    IOEngine engine;
    engine.init();

    TcpLink link;
    link.connect( "127.0.0.1", 8000);
    link.send("good", 4);

    ClientTimer timer(&link);

    engine.run();

    return 0;
}