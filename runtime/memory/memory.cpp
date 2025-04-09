#include "memory.h"

#include "EASTL/allocator.h"
#include "EASTL/compare.h"

namespace source_runtime::memory
{
	void* allocator_allocate(size_t size, size_t alignment) noexcept
	{
		return eastl::GetDefaultAllocator()->allocate(size, alignment, 0u);
	}

	void allocator_deallocate(void* p, size_t alignment) noexcept
	{
		eastl::GetDefaultAllocator()->deallocate(p, alignment);
	}
}
