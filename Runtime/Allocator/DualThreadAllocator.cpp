#include "SourcePrefix.h"
#include "DualThreadAllocator.h"

#include "AllocationHeader.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/DynamicHeapAllocator.h"
#include "Runtime/Threads/CurrentThread.h"
#include "Runtime/Allocator/MemoryMacros.h"
#include "Runtime/Threads/Mutex.h"

class DelayedPointerDeletionManager
{
public:
	DelayedPointerDeletionManager(BaseAllocator* mainAlloc, BaseAllocator* threadAlloc) :
		m_MainThreadPendingPointers(nullptr),
		m_MainThreadPendingCount(0),
		m_MainThreadPendingReserved(0),
		m_PeakMainThreadPendingCount(0),
		m_HasPendingDeletes(0),
		m_MainAllocator(mainAlloc),
		m_ThreadAllocator(threadAlloc)
	{}

	bool HasPending() { return m_HasPendingDeletes != 0; }
	void CleanupPendingMainThreadPointers();
private:
	void GrowPendingBuffer();
	void** m_MainThreadPendingPointers;
	UInt32 m_MainThreadPendingCount;
	UInt32 m_MainThreadPendingReserved;
	UInt32 m_PeakMainThreadPendingCount;

	volatile int m_HasPendingDeletes;
	BaseAllocator* m_MainAllocator;
	BaseAllocator* m_ThreadAllocator;
	Mutex m_MainThreadPendingPointersMutex;
	void CleanupPendingMainThreadPointersInternal();
};

void DelayedPointerDeletionManager::CleanupPendingMainThreadPointers()
{
	Mutex::AutoLock autolock(m_MainThreadPendingPointersMutex);
	CleanupPendingMainThreadPointersInternal();
}

void DelayedPointerDeletionManager::GrowPendingBuffer()
{
	const UInt32 kInitialBufferSize = 128;

	size_t oldReservedSize = m_MainThreadPendingReserved;

	m_MainThreadPendingReserved = std::max(m_MainThreadPendingReserved * 2, kInitialBufferSize);

}

void DelayedPointerDeletionManager::CleanupPendingMainThreadPointersInternal()
{
	Assert(CurrentThread::IsMainThread());
	m_HasPendingDeletes = 0;
	for (UInt32 i = 0; i < m_MainThreadPendingCount; ++i)
		m_MainAllocator->Deallocate(m_MainThreadPendingPointers[i]);
	
}

template <class UnderlyingAllocator>
void* DualThreadAllocator<UnderlyingAllocator>::LowLevelAllocator::ReserveMemoryBlock(size_t size, BlockInfo info)
{
	info.secondaryAllocatorIdentifier = info.allocatorIdentifier;
	info.allocatorIdentifier = m_Identifier;
	return m_SystemLLAlloc->ReserveMemoryBlock(size, info);
}

template <class UnderlyingAllocator>
void DualThreadAllocator<UnderlyingAllocator>::LowLevelAllocator::ReleaseMemoryBlock(void* ptr, size_t size)
{
	m_SystemLLAlloc->ReleaseMemoryBlock(ptr, size);
}

template <class UnderlyingAllocator>
size_t DualThreadAllocator<UnderlyingAllocator>::LowLevelAllocator::CommitMemory(void* ptr, size_t size)
{
	return m_SystemLLAlloc->CommitMemory(ptr, size);
}

template <class UnderlyingAllocator>
size_t DualThreadAllocator<UnderlyingAllocator>::LowLevelAllocator::DecommitMemory(void* ptr, size_t size)
{
	return m_SystemLLAlloc->DecommitMemory(ptr, size);
}

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
	, m_MainAllocator((UnderlyingAllocator*)mainAllocator)
	, m_ThreadAllocator((UnderlyingAllocator*)threadAllocator)
{
	m_LowLevelAllocator.Initialize(llAlloc, m_AllocatorIdentifier);
	m_MainAllocator->SetLowLevelAllocator(&m_LowLevelAllocator);
	m_ThreadAllocator->SetLowLevelAllocator(&m_LowLevelAllocator);

	m_DelayedDeletion = new (m_MainAllocator->Allocate(sizeof(DelayedPointerDeletionManager), kDefaultMemoryAlignment)) DelayedPointerDeletionManager(m_MainAllocator, m_ThreadAllocator);
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
	if (m_BucketAllocator != nullptr && m_BucketAllocator->BucketAllocator::CanAllocate(size, align))
	{
		void* ptr = m_BucketAllocator->BucketAllocator::Allocate(size, align);
		if (ptr != nullptr)
			return ptr;
	}

	UnderlyingAllocator* alloc = GetCurrentAllocator();
	bool isMainThread = alloc == m_MainAllocator;
	if (isMainThread && m_DelayedDeletion->HasPending())
		m_DelayedDeletion->CleanupPendingMainThreadPointers();

	return alloc->UnderlyingAllocator::Allocate(size, align);
}

template <class UnderlyingAllocator>
bool DualThreadAllocator<UnderlyingAllocator>::Contains(const void* p) const
{
	return true;
}

template <class UnderlyingAllocator>
size_t DualThreadAllocator<UnderlyingAllocator>::GetPtrSize(const void* ptr) const
{
	size_t size = m_BucketAllocator != nullptr ? m_BucketAllocator->BucketAllocator::GetPtrSize(ptr) : 0;
	if (size != 0)
		return size;

	return m_MainAllocator->UnderlyingAllocator::GetPtrSize(ptr);
}

template <class UnderlyingAllocator>
size_t DualThreadAllocator<UnderlyingAllocator>::get_requested_ptr_size(const void* ptr) const
{
	return AllocationHeader::GetAllocationHeader(ptr)->GetAllocationSize();
}

template class DualThreadAllocator<DynamicHeapAllocator>;