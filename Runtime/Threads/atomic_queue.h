#pragma once

#include "atomic_node.h"
#include "extended_atomic_types.h"

class alignas(PLATFORM_CACHE_LINE_SIZE)atomic_stack
{
	volatile atomic_word2 top_;
public:
	atomic_stack();
	void push(atomic_node* node);
};
