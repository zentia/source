#include <EASTL/allocator.h>

#include "mimalloc.h"

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
		return mi_malloc(n);
	}

	void* allocator::allocate(size_t n, size_t alignment, size_t offset [[maybe_unused]], int flags)
	{
		EASTL_ASSERT(offset == 0u);
		if (alignment <= EASTL_SYSTEM_ALLOCATOR_MIN_ALIGNMENT)
		{
			return allocate(n, flags);
		}
		return mi_aligned_alloc(alignment, n);
	}

	void allocator::deallocate(void* p, size_t n)
	{
		mi_free(p);
	}

}
