#pragma once

#include <stdint.h>

using atomic_word = intptr_t;

union atomic_word2
{
	__m128 v;

	struct 
	{
		atomic_word lo, hi;
	};
};
#define ATOMIC_HAS_DCAS