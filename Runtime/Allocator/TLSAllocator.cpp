#include "SourcePrefix.h"
#include "TLSAllocator.h"
#include "Runtime/Allocator/MemoryManager.h"

TLSAllocatorBase::TLSAllocatorBase(const char* name, bool isThreadSafe)
	: BaseAllocator(name, isThreadSafe)
{

}


template<AllocatorMode allocMode>
SOURCE_TLS_VALUE(typename TLSAllocator<allocMode>::StackAllocator_t*) TLSAllocator<allocMode>::m_UniqueThreadAllocator;

template <AllocatorMode allocMode>
TLSAllocator<allocMode>::TLSAllocator(LowLevelVirtualAllocator* llAlloc, const char* name)
	: TLSAllocatorBase(name, true)
{

}

template <AllocatorMode allocMode>
void* TLSAllocator<allocMode>::Allocate(size_t size, int align)
{
	StackAllocator_t* alloc = GetThreadAllocator();
	if (OPTIMIZER_UNLIKELY(!alloc))
	{
		return nullptr;
	}
	return alloc->StackAllocator_t::Allocate(size, align);
}

template <AllocatorMode allocMode>
bool TLSAllocator<allocMode>::Contains(const void* p) const
{
	StackAllocator_t* alloc = GetThreadAllocator();
	if (alloc && alloc->StackAllocator_t::Contains(p))
		return true;
	return false;
}

template <AllocatorMode allocMode>
void TLSAllocator<allocMode>::Deallocate(void* p)
{
	StackAllocator_t* alloc = GetThreadAllocator();
	if (OPTIMIZER_UNLIKELY(!alloc))
	{
		return;
	}
}

template <AllocatorMode allocMode>
size_t TLSAllocator<allocMode>::GetPtrSize(const void* ptr) const
{
	return GetThreadAllocator()->StackAllocator_t::GetPtrSize(ptr);
}

template <AllocatorMode allocMode>
size_t TLSAllocator<allocMode>::get_requested_ptr_size(const void* ptr) const
{
	return GetThreadAllocator()->StackAllocator_t::get_requested_ptr_size(ptr);
}

template <AllocatorMode allocMode>
typename TLSAllocator<allocMode>::StackAllocator_t* TLSAllocator<allocMode>::GetThreadAllocator() const
{
	return m_UniqueThreadAllocator.IsValid() ? (StackAllocator_t*)m_UniqueThreadAllocator : nullptr;
}

template class TLSAllocator<AllocatorMode::Normal>;
template class TLSAllocator<AllocatorMode::TrackLeaks>;