#pragma once

#include "LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/BaseAllocator.h"

#if ENABLE_MEMORY_MANAGER
class BucketAllocator : public BaseAllocator
{
public:
	BucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount, LowLevelVirtualAllocator* llAlloc);
	void* Allocate(size_t size, int align) override;
	size_t GetPtrSize(const void* ptr) const override;
	FORCE_INLINE bool CanAllocate(size_t size, int align) const { return size <= m_MaxBucketSize && align <= kMaxAlignment; }
	FORCE_INLINE bool ContainsPtr(const void* p) const;
private:
	struct Block
	{
		Block(int bs) : bucketSize(bs){}
		int bucketSize;
	};
	FORCE_INLINE Block* GetBlockFromPtr(const void* ptr) const { return reinterpret_cast<Block*>(((size_t)ptr) & ~(kBlockSize - 1)); }
	static const int kMaxAlignment = 16;
	static const int kBlockSize = 16 * 1024;

	const int m_BucketGranularity;
	const int m_MaxBucketSize;
	size_t m_ReserveSize;
	void* m_ReservePointer;

};
#endif
