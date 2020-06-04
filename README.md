# somo_server_netbase
# C++ wrapper on libuv for easy usage, including following components:

1. tcp+udp server based on epoll;
2. tcp+udp client based on epoll;
3. multiple components in same thread, like a process support both tcp-server on port 8000, udp server on port 8001, and tcp link connect to another tcp server.
4, timers on epoll;
5. internal keep alive check;
6, single thread model only, no multi-threads components;

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
soon later!
 
