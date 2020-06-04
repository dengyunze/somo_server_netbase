# somo_server_netbase
# C++ warpper server side network library based on libuv for easy usage. Including following components:

1. tcp server based on epoll;
2. udp server based on epoll;
3. tcp client based on epoll;
4. udp client based on epoll;
5. multiple components in same process, like a process support both tcp-server on port 8000, udp server on port 8001, and tcp link connect to another tcp server.


# Scenario:
1. create an udp server:
      class ServerHandler : public ISNLinkHandler {
      public:
          virtual int  on_data(const char* data, size_t len, uint32_t ip, short port) {
              m_nRecvs++;
              if( m_nRecvs%100 == 0 ) {
                  FUNLOG(Info, "udp server handler on data, len=%d", len);
              }
          }
      private:
          int     m_nRecvs;
      };

      int main(int argc, char* argv[]) {
          SNStartup();

          ServerHandler handler;

          ISNUdpServer* server = SNLinkFactory::createUdpServer();
          server->set_handler(&handler);
          server->listen(8000);

          SNLoop();

          return 0;
      }
      
2. create an udp client:
      int main(int argc, char* argv[]) {
          SNStartup();

          ISNLink* link = SNLinkFactory::createUdpLink();
          link->connect( "127.0.0.1", 8000);
          link->send("good", 4);

          ClientTimer timer(link);

          SNLoop();

          return 0;
      }
      
 # Benchmark
 I will add this soon later.
 
