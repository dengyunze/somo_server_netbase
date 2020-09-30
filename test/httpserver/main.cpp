#include "isnet.h"

#include <stdlib.h>

#define __CLASS__ "ServerHandler"

class ServerHandler : public ISNHttpServerHandler {
public:
    ServerHandler() {

    }

public:
    virtual void  on_get(const std::string& url, const std::map<std::string, std::string>& headers, std::string& res) {
        NETLOG(Info, "http on get, url=%s", url.c_str());
        res = "{\"status\":1}";
    }

    virtual void    on_post(const std::string& url, const std::map<std::string, std::string>& headers, const std::string& body, std::string& res) {
        NETLOG(Info, "http on post, url=%s", url.c_str());
        res = "{\"status\":1}";
    }
};

int main(int argc, char* argv[]) {
    SNStartup();

    ServerHandler handler;

    ISNHttpServer* server = SNFactory::createHttpServer();
    server->set_handler(&handler);
    server->listen(8000);

    SNLoop();

    return 0;
}