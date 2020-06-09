#pragma once

#include "singleton.h"
#include <vector>

class MemItem;
class MemPool : public Singleton<MemPool>
{   
public:
	MemPool();
	~MemPool();

public:
	MemItem*	get(int size);
	void		free(MemItem* pFrame);
    void        reset();

private:
	typedef std::vector<MemItem*>	frame_pool_t;
	frame_pool_t	m_arr4kPool;
	frame_pool_t	m_arr16kPool;
	frame_pool_t	m_arr64kPool;
	frame_pool_t	m_arr128kPool;
	int			m_nGetCount;
	int			m_nFreeCount;
};

