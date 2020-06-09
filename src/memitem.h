#pragma once

#include <stdint.h>

class MemItem
{
public:
	MemItem(int capacity);
	~MemItem();

public:
	void	assign(const char* data, int len);
	void	clear();

public:
	inline	const char*	data() { return m_pData; }
	inline 	uint32_t	len() { return m_nLen; }
	inline  uint32_t	capacity() { return m_nCapacity; }

private:
	char*		m_pData;
	int			m_nLen;
	int			m_nCapacity;
};