#include "memitem.h"
#include "logger.h"

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define __CLASS__ "MemItem"

MemItem::MemItem(int capacity) 
: m_nCapacity(capacity)
, m_nLen(0)
, m_pData(NULL)
{
	if( capacity == 0 ) {
		FUNLOG(Error, "mem item constructor, capacity==0!!!", NULL);
		return;
	}
	m_pData = (char*)malloc(capacity);
}

MemItem::~MemItem() {
	if( m_pData != NULL ) {
		free(m_pData);
	}
}

void	MemItem::assign(const char* data, int len) {
	if( len <= 0 || data == NULL ) {
		FUNLOG(Error, "mem item assign, data==NULL or len==0, len=%d", len);
		return;
	}

	if( len>1024*1024 ) {
		FUNLOG(Error, "mem item, TOO LARGE packet, len=%d", len);
		return;
	}
	if( len > m_nCapacity ) {
		FUNLOG(Warn, "mem item assign, len>capacity, len=%d, capacity=%d", len, m_nCapacity);
		if( m_pData != NULL ) {
			free(m_pData);
		}
		m_pData = (char*)malloc(len);
		m_nCapacity = len;
	}

	m_nLen = len;
	memcpy(m_pData, data, len);
}

void	MemItem::clear() {
	m_nLen = 0;
}

