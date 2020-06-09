#pragma once

#include <stdint.h>
#include <set>

class LinkidAllocator
{

public:
    static uint32_t next();

private:
    static uint32_t s_nLastId;
};

