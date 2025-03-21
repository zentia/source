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

	void InitializeTLSF();

	void* Allocate(size_t size, int align) override;
	void Deallocate(void* ptr) override;
	bool Contains(const void* p) const override;
	size_t GetPtrSize(const void* ptr) const override;
	static constexpr bool IsMTECompliant() { return true; }

private:
	
	struct MemoryBlockInfo:public ListElement
	{
		size_t allocationCount;
	};

	struct LargeAllocInfo : public ListElement
	{
		size_t allocatedSize;
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
		// TLSF Block
		List<MemoryBlockInfo> m_TLSFMemoryBlocks;
		SInt16* m_MemoryBlockFreeList;
		SInt16 m_NextFreeMemoryBlockIndex;
		// LargeAlloc Block
		List<LargeAllocInfo> m_LargeAllocs;
		size_t m_LargeAllocCount;
		size_t m_RegionSize;
	};

	List<MemoryRegionInfo> m_MemoryRegions;

	void* ReserveMemoryRegion(size_t size, regionType type);

	void InitializeMemoryRegion(void* memoryBlock, regionType type, size_t size);
	bool IsTLSFBlock(const void* ptr) const;

	void* CreateTLSFBlock(size_t& blockSize);
	void* GetMemoryRegionPointer(const void* ptr) const { return m_LLAlloc->GetMemoryBlockFromPointer(ptr); }
	MemoryRegionInfo* GetMemoryRegionInfo(const void* ptr) const { return (MemoryRegionInfo*)((UInt64)GetMemoryRegionPointer(ptr)); }
	BucketAllocator* m_BucketAllocator;
	tlsf_t m_TlsfInstance;

	void* GetBlockPointer(const void* ptr) const { return (void*)((UInt64)ptr & (~(m_RequestedBlockSize - 1))); }
	MemoryBlockInfo* GetBlockInfo(const void* ptr) const { return (MemoryBlockInfo*)((UInt64)GetBlockPointer(ptr) + m_RequestedBlockSize - sizeof(MemoryBlockInfo)); }

	mutable Mutex m_DHAMutex;
	bool m_UseLocking;

	size_t GetControlStructureSize() const;

	UInt32 m_BlocksPerRegion;
	static const size_t kBlockSizeMax = 256 * 1024 * 1024;
	static const size_t kBlockSizeMin = 128 * 1024;

	size_t m_RequestedBlockSize;

	LowLevelVirtualAllocator* m_LLAlloc;
	size_t m_LowLevelReservedMemory;
};
