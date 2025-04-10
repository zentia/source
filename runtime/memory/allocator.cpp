#include <time.h>
#include "ff/allocator.hpp"

#include <EASTL/allocator.h>

namespace eastl
{
	allocator* GetDefaultAllocator()
	{
		static allocator a;
		return &a;
	}

	allocator::allocator(const char* pName)
	{
		
	}

	void* allocator::allocate(size_t n, int flags)
	{
		return ff::ff_malloc(n);
	}

	void* allocator::allocate(size_t n, size_t [[maybe_unused]] alignment, size_t offset [[maybe_unused]], int flags)
	{
		return ff::ff_malloc(n);
	}

	void allocator::deallocate(void* p, size_t n)
	{
		ff::ff_free(p);
	}

}
