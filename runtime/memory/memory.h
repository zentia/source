#pragma once

namespace source_runtime::memory
{
	void* allocator_allocate(size_t size, size_t alignment) noexcept;
	void allocator_deallocate(void* p, size_t alignment) noexcept;
}
