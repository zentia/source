#include "SourcePrefix.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryMacros.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "Runtime/Utitlities/StaticAssert.h"
#include "Runtime/Utitlities/Utility.h"

BucketAllocator::BucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount, LowLevelVirtualAllocator* llAlloc)
	: BaseAllocator(name)
	, m_BucketGranularity(clamp<size_t>(bucketGranularity, sizeof(size_t), 256))
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
