#include "linkhandlerbase.h"
#include "uni.h"
#include "logger.h"

#define __CLASS__ "LinkHandlerBase"

LinkHandlerBase::LinkHandlerBase() {

}

LinkHandlerBase::~LinkHandlerBase() {
    
}

int LinkHandlerBase::on_data(const char* data, size_t len, ISNLink* pLink)
{
    if( data == NULL )
        return 0;
    
    int remains = len;
    int handleLen = 0;
    while ( remains >= 4 ) {
        const char* temp = data + handleLen;
        uint32_t peekLen = uni::peek_len(temp);
        //log(Debug, "tcp data, len:%u peekLen:%u", len, peekLen);
        if ( remains < peekLen )
            break;
        
        if( peekLen <= 12 ) {
            FUNLOG(Error, "link handler base, data<12, peeklen=%d, len=%d, x[0]=%x, x[1]=%x, x[2]=%x, x[3]=%x, x[4]=%x, x[5]=%x", 
                peekLen, len, temp[0], temp[1], temp[2], temp[3], temp[4], temp[5]);
            break;
        }
        if( peekLen > 1024*512 ) {
            FUNLOG(Error, "link handler base, data>512K, peeklen=%d, len=%d", peekLen, len);
            break;
        }

        try {
            Unpack up(temp, peekLen);
            up.pop_header();
            handle_protocol(up, pLink, temp, peekLen);
        } catch(UnpackError ex) {
            FUNLOG(Error, "handle protocol failed, msg=%s", ex.what());
            break;
        }
        handleLen += peekLen;
        remains -= peekLen;
    }
    return handleLen;
}

void LinkHandlerBase::answer(ISNLink* pLink, uint32_t uri, const Marshallable& m, uint16_t appid)
{
    Pack _p(uri, appid);
    m.marshal(_p);
    _p.push_header();
    pLink->send(_p.header(), _p.header_size()+_p.body_size());
}
