#pragma once

#include "packet.h"
#include "unpack.h"
#include "comm.h"
#include "packbuf.h"

#include <stdint.h>
#include <sstream>
#include <set>
#include <map>

struct Marshallable
{
    virtual void marshal(Pack& pk) const = 0;
    virtual void unmarshal(const Unpack& up) = 0;
    virtual void unmarshal_incomplete(const Unpack& up){}
};

inline Pack& operator << (Pack& p, const Marshallable& m)
{
    Pack _p(0);
    m.marshal(_p);
    _p.push_header(); //push header
    p.push(_p.header(),_p.header_size()+_p.body_size());
    return p;
}

inline const Unpack& operator >> (const Unpack& up, Marshallable& m)
{
    std::string innerPacket = up.pop_packet(); //length 32bits
    Unpack _up(innerPacket.data(),innerPacket.size());
    _up.pop_header();
    m.unmarshal(_up);
    return up;
}

