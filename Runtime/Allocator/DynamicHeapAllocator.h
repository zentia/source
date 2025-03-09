#pragma once

#include "BaseAllocator.h"
#include "LowLevelDefaultAllocator.h"
#include "External/Allocator/tlsf/tlsf.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Utitlities/LinkedList.h"

class BucketAllocator;

class DynamicHeapAllocator : public BaseAllocator
{
public:
	DynamicHeapAllocator(UInt32 memoryBlockSize, bool useLocking, BucketAllocator* bucketAllocator, LowLevelVirtualAllocator* llAlloc, const char* name, bool checkEmptyOnDestruct = true);

	void* Allocate(size_t size, int align) override;
	void Deallocate(void* ptr) override;
	bool Contains(const void* p) const override;
	size_t GetPtrSize(const void* ptr) const override;
	static constexpr bool IsMTECompliant() { return true; }

private:
	
	struct MemoryBlockInfo:public ListElement
	{
		
	};
	enum regionType
	{
		kTLSFBlocks,
		kLargeAlloc,
		kMixed32Bit,
	};
	struct MemoryRegionInfo : public ListElement
	{
		regionType m_Type;
		List<MemoryBlockInfo> m_TLSFMemoryBlocks;

	};
	bool IsTLSFBlock(const void* ptr) const;
	void* GetMemoryRegionPointer(const void* ptr) const { return m_LLAlloc->GetMemoryBlockFromPointer(ptr); }
	MemoryRegionInfo* GetMemoryRegionInfo(const void* ptr) const { return (MemoryRegionInfo*)((UInt64)GetMemoryRegionPointer(ptr)); }
	BucketAllocator* m_BucketAllocator;
	tlsf_t m_TlsfInstance;

	mutable Mutex m_DHAMutex;
	bool m_UseLocking;
	static const size_t kBlockSizeMax = 256 * 1024 * 1024;
	static const size_t kBlockSizeMin = 128 * 1024;
	LowLevelVirtualAllocator* m_LLAlloc;
};
