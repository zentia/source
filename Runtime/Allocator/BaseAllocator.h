#pragma once

#include "baselib/Include/Cpp/Atomic.h"
#include "Runtime/Misc/AllocatorLabels.h"
#include "Runtime/Utitlities/NonCopyable.h"

enum class AllocatorMode : std::uint8_t
{
	Normal,
	TrackLeaks,
};

class BaseAllocator : public NonCopyable
{
public:
	BaseAllocator(const char* name, bool isThreadSafe = false);

	virtual void* Allocate(size_t size, int align) = 0;
	virtual void Deallocate(void* p) = 0;
	virtual bool TryDeallocate(void* p);
	virtual bool Contains(const void* p) const = 0;
	virtual size_t GetPtrSize(const void* ptr) const = 0;
	virtual bool IsAssigned() const { return true; }
protected:
	void RegisterAllocationData(size_t allocatedSize, size_t overhead);
	void RegisterReallocationData(size_t oldAllocatedSize, size_t oldOverhead, size_t allocatedSize, size_t overhead);
	void RegisterDeallocationData(size_t allocatedSize, size_t overhead);

	void RegisterMemoryIncrease(size_t allocatedSize);
	static constexpr  int kMaxNameLength = 64;
	char m_Name[kMaxNameLength];
	UInt16 m_AllocatorIdentifier;

	UInt32 m_NumAllocations;
	size_t m_TotalAllocatedBytes;
	size_t m_TotalReservedBytes;
	size_t m_PeakAllocatedBytes;
	mutable size_t m_BookKeepingMemoryUsage;

	static constexpr  int kInitialSizeGroupBitShift = 9; // place all sizes less than 512KB in first bucket
	size_t m_FramePeakAllocatedBytes;
	int m_FramePeakUsageRangeCount[32];
private:
	bool m_IsThreadSafe; // false for allocators that need a lock, true for allocators that are inherently thread safe
public:
	bool NeverBlock() const { return m_NeverBlock; }
	void SetNeverBlock(bool value) { m_NeverBlock = value; }

	bool CollectCallstack() const { return m_CollectCallstack && !NeverBlock(); }
	void SetCollectCallstack(bool value) { m_CollectCallstack = value; }
protected:
	bool m_NeverBlock;
	bool m_CollectCallstack;
};

inline void BaseAllocator::RegisterAllocationData(size_t allocatedSize, size_t overhead)
{
	baselib::atomic_fetch_add_explicit(m_BookKeepingMemoryUsage, overhead, baselib::memory_order_relaxed);
	RegisterMemoryIncrease(allocatedSize);
	baselib::atomic_fetch_add_explicit(m_NumAllocations, 1, baselib::memory_order_relaxed);
}

inline void BaseAllocator::RegisterReallocationData(size_t oldAllocatedSize, size_t oldOverhead, size_t allocatedSize, size_t overhead)
{
	if (oldOverhead > overhead)
		baselib::atomic_fetch_sub_explicit(m_BookKeepingMemoryUsage, oldOverhead - overhead, baselib::memory_order_relaxed);
	else if (oldOverhead < overhead)
		baselib::atomic_fetch_add_explicit(m_BookKeepingMemoryUsage, overhead - oldOverhead, baselib::memory_order_relaxed);

	if (oldAllocatedSize > allocatedSize)
		baselib::atomic_fetch_sub_explicit(m_TotalAllocatedBytes, oldAllocatedSize - allocatedSize, baselib::memory_order_relaxed);
	else if (oldAllocatedSize < allocatedSize)
		RegisterMemoryIncrease(allocatedSize - oldAllocatedSize);
}


inline void BaseAllocator::RegisterDeallocationData(size_t allocatedSize, size_t overhead)
{
	baselib::atomic_fetch_sub_explicit(m_TotalAllocatedBytes, allocatedSize, baselib::memory_order_relaxed);
	baselib::atomic_fetch_sub_explicit(m_BookKeepingMemoryUsage, overhead, baselib::memory_order_relaxed);
	baselib::atomic_fetch_sub_explicit(m_NumAllocations, 1, baselib::memory_order_relaxed);
}

inline void BaseAllocator::RegisterMemoryIncrease(size_t allocatedSize)
{
	size_t oldTotalAllocatedBytes = baselib::atomic_fetch_add_explicit(m_TotalAllocatedBytes, allocatedSize, baselib::memory_order_relaxed);
	size_t totalAllocatedBytesIncludingThisAllocation = oldTotalAllocatedBytes + allocatedSize;

	size_t localPeakAllocatedBytes = baselib::atomic_load_explicit(m_PeakAllocatedBytes, baselib::memory_order_relaxed);
	while (totalAllocatedBytesIncludingThisAllocation > localPeakAllocatedBytes)
	{
		if (baselib::atomic_compare_exchange_weak_explicit(m_PeakAllocatedBytes, localPeakAllocatedBytes, totalAllocatedBytesIncludingThisAllocation, baselib::memory_order_relaxed, baselib::memory_order_relaxed))
			break;
	}
	size_t localFramePeakAllocatedBytes = baselib::atomic_load_explicit(m_FramePeakAllocatedBytes, baselib::memory_order_relaxed);
	while (totalAllocatedBytesIncludingThisAllocation > localFramePeakAllocatedBytes)
	{
		if (baselib::atomic_compare_exchange_weak_explicit(m_FramePeakAllocatedBytes, localFramePeakAllocatedBytes, totalAllocatedBytesIncludingThisAllocation, baselib::memory_order_relaxed, baselib::memory_order_relaxed))
			break;
	}
}
