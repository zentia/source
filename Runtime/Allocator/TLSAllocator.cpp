#include "SourcePrefix.h"
#include "TLSAllocator.h"
#include "Runtime/Allocator/MemoryManager.h"

template<AllocatorMode allocMode>
SOURCE_TLS_VALUE(typename TLSAllocator<allocMode>::StackAllocator_t*) TLSAllocator<allocMode>::m_UniqueThreadAllocator;

template <AllocatorMode allocMode>
bool TLSAllocator<allocMode>::Contains(const void* p) const
{
	StackAllocator_t* alloc = GetThreadAllocator();
	if (alloc && alloc->StackAllocator_t::Contains(p))
		return true;
	return false;
}

template <AllocatorMode allocMode>
typename TLSAllocator<allocMode>::StackAllocator_t* TLSAllocator<allocMode>::GetThreadAllocator() const
{
	return m_UniqueThreadAllocator.IsValid() ? (StackAllocator_t)m_UniqueThreadAllocator : nullptr;
}
