#include "mempool.h"
#include "memitem.h"
#include "logger.h"
#include <stdlib.h>

#define __CLASS__ "MemPool"

#define MEM_ITEM_SIZE_4K		1024*4
#define MEM_ITEM_SIZE_16K		1024*16
#define MEM_ITEM_SIZE_64K		1024*64
#define MEM_ITEM_SIZE_128K		1024*128
#define MEM_ITEM_SIZE_MAX		1024*512

#define MEM_ITEM_COUNT_MAX		1000

#define __CLASS__ "MemPool"

MemPool::MemPool()
: m_nGetCount(0)
, m_nFreeCount(0)
{
}

MemPool::~MemPool()
{
    reset();
}


MemItem* MemPool::get(int size)
{
    m_nGetCount++;
    if( m_nGetCount%300000 == 0 ) {
        FUNLOG(Info, "mem pool get, m_nGetCount=%d, m_nFreeCount=%d", m_nGetCount, m_nFreeCount);
    }

	MemItem* pFrame = NULL;
	if( size <= MEM_ITEM_SIZE_4K ) {
		if ( !m_arr4kPool.empty() ) {
			pFrame = m_arr4kPool.back();
			m_arr4kPool.pop_back();
		}
		else {
			pFrame = new MemItem(MEM_ITEM_SIZE_4K);
		}
	} else if( size<=MEM_ITEM_SIZE_16K ) {
		if ( !m_arr16kPool.empty() ) {
			pFrame = m_arr16kPool.back();
			m_arr16kPool.pop_back();
		}
		else {
			pFrame = new MemItem(MEM_ITEM_SIZE_16K);
		}
	} else if( size<=MEM_ITEM_SIZE_64K) {
		if ( !m_arr64kPool.empty() ) {
			pFrame = m_arr64kPool.back();
			m_arr64kPool.pop_back();
		}
		else {
			pFrame = new MemItem(MEM_ITEM_SIZE_64K);
		}
	} else if( size<=MEM_ITEM_SIZE_128K ) {
		if ( !m_arr128kPool.empty() ) {
			pFrame = m_arr128kPool.back();
			m_arr128kPool.pop_back();
		}
		else {
			pFrame = new MemItem(MEM_ITEM_SIZE_128K);
		}
	} else if( size<=MEM_ITEM_SIZE_MAX ) {
		pFrame = new MemItem(size);
	} else {
		FUNLOG(Error, "mem pool get, TOO large size=%d", size);
		return NULL;
	}

	return pFrame;
}


void MemPool::free(MemItem* pFrame)
{
	if( pFrame == NULL )
		return;

    m_nFreeCount++;
	if( m_nFreeCount <= 3 ) {
		FUNLOG(Info, "mem pool free, len=%d, capacity=%d", pFrame->len(), pFrame->capacity());
	}
	pFrame->clear();

	if( pFrame->capacity() <= MEM_ITEM_SIZE_4K ) {
		if ( m_arr4kPool.size() > MEM_ITEM_COUNT_MAX ) {
			delete pFrame;
			pFrame = NULL;
			return;
		}
		m_arr4kPool.push_back(pFrame);
	} else if( pFrame->capacity() <= MEM_ITEM_SIZE_16K ) {
		if ( m_arr16kPool.size() > MEM_ITEM_COUNT_MAX ) {
			delete pFrame;
			pFrame = NULL;
			return;
		}
		m_arr16kPool.push_back(pFrame);
	} else if( pFrame->capacity() <= MEM_ITEM_SIZE_64K ) {
		if ( m_arr64kPool.size() > MEM_ITEM_COUNT_MAX ) {
			delete pFrame;
			pFrame = NULL;
			return;
		}
		m_arr64kPool.push_back(pFrame);
	} else if( pFrame->capacity() <= MEM_ITEM_SIZE_128K ) {
		if ( m_arr128kPool.size() > MEM_ITEM_COUNT_MAX ) {
			delete pFrame;
			pFrame = NULL;
			return;
		}
		m_arr64kPool.push_back(pFrame);
	} else {
		delete pFrame;
	}
}

void MemPool::reset() {
    for ( frame_pool_t::iterator io = m_arr4kPool.begin(); io != m_arr4kPool.end(); ++io ) {
        delete (*io);
    }
    m_arr4kPool.clear();

	for ( frame_pool_t::iterator io = m_arr16kPool.begin(); io != m_arr16kPool.end(); ++io ) {
        delete (*io);
    }
    m_arr16kPool.clear();

	for ( frame_pool_t::iterator io = m_arr64kPool.begin(); io != m_arr64kPool.end(); ++io ) {
        delete (*io);
    }
    m_arr64kPool.clear();

	for ( frame_pool_t::iterator io = m_arr128kPool.begin(); io != m_arr128kPool.end(); ++io ) {
        delete (*io);
    }
    m_arr128kPool.clear();
}
