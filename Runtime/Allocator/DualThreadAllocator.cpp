#include "SourcePrefix.h"
#include "DualThreadAllocator.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/DynamicHeapAllocator.h"
#include "Runtime/Threads/CurrentThread.h"

template <class UnderlyingAllocator>
LowLevelVirtualAllocator::BlockInfo DualThreadAllocator<UnderlyingAllocator>::LowLevelAllocator::GetBlockInfoFromPointer(const void* ptr)
{
	BlockInfo info = m_SystemLLAlloc->GetBlockInfoFromPointer(ptr);
	info.allocatorIdentifier = info.secondaryAllocatorIdentifier;
	return info;
}


template <class UnderlyingAllocator>
DualThreadAllocator<UnderlyingAllocator>::DualThreadAllocator(const char* name, BucketAllocator* bucketAllocator, BaseAllocator* mainAllocator, BaseAllocator* threadAllocator, LowLevelVirtualAllocator* llAlloc)
	: BaseAllocator(name)
	, m_BucketAllocator(bucketAllocator)
{
	
}

template <class UnderlyingAllocator>
UnderlyingAllocator* DualThreadAllocator<UnderlyingAllocator>::GetCurrentAllocator() const
{
	if (CurrentThread::IsMainThread())
		return m_MainAllocator;
	else
		return m_ThreadAllocator;
}

template <class UnderlyingAllocator>
void* DualThreadAllocator<UnderlyingAllocator>::Allocate(size_t size, int align)
{
	// Try lockless allocator first
	if (m_BucketAllocator != nullptr && m_BucketAllocator->BucketAllocator::CanAllocate(size, align))
	{
		void* ptr = m_BucketAllocator->BucketAllocator::Allocate(size, align);
		if (ptr != nullptr)
			return ptr;
	}

	// Use either main thread allocator or thread allocator
	UnderlyingAllocator* alloc = GetCurrentAllocator();
}
