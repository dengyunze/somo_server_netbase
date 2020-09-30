# somo_server_netbase
## C++ wrapper on libuv for easy usage, including following features:

1. tcp+udp server on epoll;
2. tcp+udp client on epoll;
3. timers on epoll;
4. http server on epoll;
5. single thread loop, no multi-threads components;
6. memory pool for quick network packet copy；
7. easy to use;

## How to use
1. call SNStartup to init the environment, handle socket limit and ignore signals;
2. use SNLinkFactory to create a ISNUdpServer;
3. set a ISNLinkHandler to the ISNUdpServer, ISNLinkHandler.on_data will be called when data come;
4. call SNLoop to start epoll loop;

## Scenario:
### 1. create a server for both tcp and udp, on port 8000:

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

    ISNUdpServer* udp_server = SNFactory::createUdpServer();
    udp_server->set_handler(&handler);
    udp_server->listen(8000);

    ISNTcpServer* tcp_server = SNFactory::createTcpServer();
    tcp_server->set_handler(&handler);
    tcp_server->listen(8000);

    SNLoop();

    return 0;
}
```      

### 2. create an udp client, and send data every 1 second (with a ISNTimer).

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

      
## Benchmark
packet size | client | tps per client | cpu |
---|---|---|---
1200 | 3 | 100000 | 31.6%
1200 | 3 | 300000 | 68%
 
## Where is the logs?
tail -f /var/log/message | grep {your_app_name}

##Note:
1, This wrapper is design for single-thread server usage with highly performance requiremen;