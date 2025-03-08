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

	static constexpr bool IsMTECompliant() { return true; }
	struct LowLevelAllocator : public LowLevelVirtualAllocator
	{
		BlockInfo GetBlockInfoFromPointer(const void* ptr) override;

		LowLevelVirtualAllocator* m_SystemLLAlloc;

	};
private:
	UnderlyingAllocator* GetCurrentAllocator() const;

	BucketAllocator* m_BucketAllocator;
	UnderlyingAllocator* m_MainAllocator;
	UnderlyingAllocator* m_ThreadAllocator;

	DelayedPointerDeletionManager* m_DelayedDeletion;
	LowLevelAllocator m_LowLevelAllocator;
};