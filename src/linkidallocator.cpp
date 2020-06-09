#pragma once

#include "linkidallocator.h"


uint32_t LinkidAllocator::s_nLastId = 1;
uint32_t LinkidAllocator::next() {
    return s_nLastId++;
}

