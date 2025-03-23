#pragma once

#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"

class BucketAllocator;
class DelayedPointerDeletionManager;

template<class UnderlyingAllocator>
class DualThreadAllocator : public BaseAllocator
{
public:
	DualThreadAllocator(const char* name, BucketAllocator* bucketAllocator, BaseAllocator* mainAllocator, BaseAllocator* threadAllocator, LowLevelVirtualAllocator* llAlloc);
	void* Allocate(size_t size, int align) override;
	bool Contains(const void* p) const override;
	void Deallocate(void* p) override { TryDeallocate(p); };
	size_t GetPtrSize(const void* ptr) const override;
	static constexpr bool IsMTECompliant() { return true; }
	struct LowLevelAllocator : public LowLevelVirtualAllocator
	{
		void* ReserveMemoryBlock(size_t size, BlockInfo info) override;
		void ReleaseMemoryBlock(void* ptr, size_t size) override;
		size_t CommitMemory(void* ptr, size_t size) override;
		size_t DecommitMemory(void* ptr, size_t size) override;
		void* GetMemoryBlockFromPointer(const void* ptrInBlock) override { return m_SystemLLAlloc->GetMemoryBlockFromPointer(ptrInBlock); }
		BlockInfo GetBlockInfoFromPointer(const void* ptr) override;
		void Initialize(LowLevelVirtualAllocator* llAlloc, UInt16 allocatorIdentifier)
		{
			m_SystemLLAlloc = llAlloc;
			m_Identifier = allocatorIdentifier;
		}

		LowLevelVirtualAllocator* m_SystemLLAlloc;
		UInt16 m_Identifier;
	};
private:
	UnderlyingAllocator* GetCurrentAllocator() const;

	BucketAllocator* m_BucketAllocator;
	UnderlyingAllocator* m_MainAllocator;
	UnderlyingAllocator* m_ThreadAllocator;

	DelayedPointerDeletionManager* m_DelayedDeletion;
	LowLevelAllocator m_LowLevelAllocator;
};