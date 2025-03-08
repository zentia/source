#pragma once

#include "BaseAllocator.h"
#include "LowLevelDefaultAllocator.h"
#include "External/Allocator/tlsf/tlsf.h"

class BucketAllocator;

class DynamicHeapAllocator : public BaseAllocator
{
public:
	DynamicHeapAllocator(UInt32 memoryBlockSize, bool useLocking, BucketAllocator* bucketAllocator, LowLevelVirtualAllocator* llAlloc, const char* name, bool checkEmptyOnDestruct = true);

	void* Allocate(size_t size, int align) override;
	static constexpr bool IsMTECompliant() { return true; }

private:
	BucketAllocator* m_BucketAllocator;
	tlsf_t m_TlsfInstance;
};
