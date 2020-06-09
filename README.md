# somo_server_netbase
## C++ wrapper on libuv for easy usage, including following components:

1. tcp+udp server on epoll;
2. tcp+udp client on epoll;
3. multiple components in same thread, like a process listen on tcp port 8000, udp port 8001, and another tcp link connect to another tcp server.
4. timers on epoll;
5. internal keep alive check;
6. single thread model only, no multi-threads components;
7. memory pool for quick network packet copy；
8. multi-read and multi-write for performance [TODO];
9. reliable udp (ARQ);
10. bandwidth check;
11. network transfer stats;

## How to use
1. call SNStartup to init the environment, include socket limit and ignore signals;
2. use SNLinkFactory to create a ISNUdpServer;
3. set a ISNLinkHandler to the ISNUdpServer, ISNLinkHandler.on_data will will be called when data come;
4. call SNLoop to start epoll loop;

## Scenario:
### 1. create an udp server:

```
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
        if( m_nRecvs%100 == 0 ) {
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

    ISNUdpServer* server = SNFactory::createUdpServer();
    server->set_handler(&handler);
    server->listen(8000);

    SNLoop();

    return 0;
}   
```      

### 2. create an udp client:

```
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
        m_pTimer->start(100);
    }

    ~ClientTimer() {
        m_pTimer->close();
    }

public:
    virtual void    on_timer(int id) {
        char* buf = new char[1200];
        memset(buf, 0, 1200);
        m_pLink->send(buf, 1200);
    }

    virtual int  on_data(const char* data, size_t len, uint32_t ip, short port) {
        NETLOG(Info, "udp link on data, len=%u", len);
    }

private:
    ISNTimer*   m_pTimer;
    ISNLink*    m_pLink;
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
```      

## packet format
LinkHandlerBase suppose the network packet start with len (4 bytes). 
      
## Benchmark
soon later!
 
## Where is the logs?
tail -f /var/log/message | grep {your_app_name}