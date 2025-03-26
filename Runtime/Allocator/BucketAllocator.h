#pragma once

#include "LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Threads/atomic_node.h"
#include "Runtime/Threads/atomic_ops.h"
#include "Runtime/Threads/atomic_queue.h"
#include "Runtime/Threads/Mutex.h"

class BucketAllocator : public BaseAllocator
{
public:
	BucketAllocator(const char* name, size_t bucketGranularity, size_t bucketsCount, size_t largeBlockSize, size_t maxLargeBlocksCount, LowLevelVirtualAllocator* llAlloc);
	void* Allocate(size_t size, int align) override;
	size_t GetPtrSize(const void* ptr) const override;
	size_t get_requested_ptr_size(const void* ptr) const override;
	bool Contains(const void* p) const override;
	void Deallocate(void* p) override;
	FORCE_INLINE bool CanAllocate(size_t size, int align) const { return size <= m_MaxBucketSize && align <= kMaxAlignment; }
	FORCE_INLINE bool ContainsPtr(const void* p) const;
	
	void DeallocateInternal(void* p);
private:
	struct Block
	{
		Block(int bs) : bucketSize(bs){}
		int bucketSize;
	};

	struct buckets : NonCopyable
	{
		buckets():used_buckets_count(0),used_blocks_count(0),max_used_buckets_count(0),buckets_size(0),real_buckets_size(0),can_grow(1),failed_allocation_count(0){}
		void push_bucket(void* p)
		{
			auto* node = static_cast<atomic_node*>(p);
			node->data[0] = nullptr;
			available_buckets.push(node);
		}
		void update_used(const int delta) const
		{
			const int new_used = atomic_add(&used_buckets_count, delta);
			if (delta <= 0)
				return;
			int new_max_used_buckets_count;
			do
			{
				new_max_used_buckets_count = atomic_add(&max_used_buckets_count, 0);
			}
			while (new_max_used_buckets_count < new_used && !atomic_compare_exchange(&max_used_buckets_count, new_used, new_max_used_buckets_count));
		}
		alignas(PLATFORM_CACHE_LINE_SIZE) atomic_stack available_buckets;
		alignas(4) mutable volatile int used_buckets_count;
		alignas(4) volatile int used_blocks_count;
		alignas(4) mutable volatile int max_used_buckets_count;
		alignas(4) volatile int can_grow;
		alignas(4) volatile int failed_allocation_count;
		int buckets_size;
		int real_buckets_size;
		Mutex grow_mutex;
	};
	FORCE_INLINE Block* GetBlockFromPtr(const void* ptr) const { return reinterpret_cast<Block*>(((size_t)ptr) & ~(kBlockSize - 1)); }
	int get_bucket_index_for_size(size_t size) const { return size > 0 ? ((size - 1) >> m_bucket_granularity_bits_) : 0; }
	buckets* get_buckets_for_size(size_t size) { return &m_buckets_[get_bucket_index_for_size(size)]; }
	static const int kMaxAlignment = 16;
	static const int kBlockSize = 16 * 1024;

	
	const int m_BucketGranularity;
	const int m_bucket_granularity_bits_;
	const int m_MaxBucketSize;

	static constexpr size_t k_max_bucket_count = 32;
	buckets m_buckets_[k_max_bucket_count];

	size_t m_ReserveSize;
	void* m_ReservePointer;

};
