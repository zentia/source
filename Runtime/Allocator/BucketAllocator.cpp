#include "SourcePrefix.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryMacros.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "Runtime/Utitlities/StaticAssert.h"

BucketAllocator::BucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount, LowLevelVirtualAllocator* llAlloc)
	: BaseAllocator(name)
	, m_BucketGranularity(clamp)
	, m_MaxBucketSize()
{
	
}
