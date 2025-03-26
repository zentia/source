#include "SourcePrefix.h"
#include "Runtime/Allocator/BucketAllocator.h"

#include "AllocationHeader.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryMacros.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "Runtime/Utitlities/BitUtility.h"
#include "Runtime/Utitlities/StaticAssert.h"
#include "Runtime/Utitlities/Utility.h"

BucketAllocator::BucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount, LowLevelVirtualAllocator* llAlloc)
	: BaseAllocator(name)
	, m_BucketGranularity(clamp<size_t>(bucketGranularity, sizeof(size_t), 256))
	, m_bucket_granularity_bits_(HighestBit(m_BucketGranularity))
	, m_MaxBucketSize()
{
	
}

void* BucketAllocator::Allocate(size_t size, int align)
{
	return nullptr;
}

size_t BucketAllocator::GetPtrSize(const void* ptr) const
{
	if (!ContainsPtr(ptr))
		return 0;

	return GetBlockFromPtr(ptr)->bucketSize;
}

bool BucketAllocator::ContainsPtr(const void* p) const
{
	return p >= m_ReservePointer && p < (char*)m_ReservePointer + m_ReserveSize;
}

bool BucketAllocator::Contains(const void* p) const
{
	return ContainsPtr(p);
}

void BucketAllocator::Deallocate(void* p)
{
	DeallocateInternal(p);
}

size_t BucketAllocator::get_requested_ptr_size(const void* ptr) const
{
	return AllocationHeader::GetAllocationHeader(ptr)->GetAllocationSize();
}

void BucketAllocator::DeallocateInternal(void* p)
{
	const AllocationHeader* allocation_header = AllocationHeader::GetAllocationHeader(p);
	void* real_ptr = allocation_header->GetAllocationPtr();
	const Block* old_block = GetBlockFromPtr(real_ptr);
	RegisterDeallocationData(allocation_header->GetAllocationSize(), old_block->bucketSize - allocation_header->GetAllocationSize());
	buckets* buckets = get_buckets_for_size(old_block->bucketSize);
	buckets->push_bucket(real_ptr);
	buckets->update_used(-1);
}
