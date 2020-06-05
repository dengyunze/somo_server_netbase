#pragma once

#include "isnet.h"
#include "packet.h"
#include "marshal.h"
#include "unpack.h"

class LinkHandlerBase : public ISNLinkHandler
{
public:
    LinkHandlerBase();
    ~LinkHandlerBase();

public:
    virtual void on_connected(ISNLink* pLink) {}
    virtual void on_close(ISNLink* pLink) {}
    virtual int  on_data(const char* data, size_t len, ISNLink* pLink);
    virtual void handle_protocol(Unpack& up, ISNLink* pLink, const char* data, size_t len) = 0;
    
public:
    void answer(ISNLink* pLink, uint32_t uri, const Marshallable& m, uint16_t appid = 0);
};

