#pragma once

#include "extended_atomic_types.h"

class atomic_node
{
	friend class atomic_stack;
	volatile atomic_word next_;
public:
	void* data[3];

	atomic_node *next() const
	{
		return (atomic_node*)next_;
	}
};
