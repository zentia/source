#pragma once

#include "BaseAllocator.h"
#include "LowLevelDefaultAllocator.h"
#include "tlsf.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Utitlities/LinkedList.h"

class BucketAllocator;

class DynamicHeapAllocator : public BaseAllocator
{
public:
	DynamicHeapAllocator(UInt32 memoryBlockSize, bool useLocking, BucketAllocator* bucketAllocator, LowLevelVirtualAllocator* llAlloc, const char* name, bool checkEmptyOnDestruct = true);

	void SetLowLevelAllocator(LowLevelVirtualAllocator* llAlloc);
	void InitializeTLSF();

	void* Allocate(size_t size, int align) override;
	void Deallocate(void* ptr) override;
	bool Contains(const void* p) const override;
	size_t GetPtrSize(const void* ptr) const override;
	size_t get_requested_ptr_size(const void* ptr) const override;
	static constexpr bool IsMTECompliant() { return true; }

private:
	
	struct MemoryBlockInfo : public ListElement
	{
		size_t allocationCount;
		// Block containing the tlsf instance can not be decommitted
		// First block of each memory region on 64 bit contains region information and should not be deleted
		bool canDecommitBlock;
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

	void ReleaseMemoryBlock(void* ptr, size_t size);
	void* ReserveMemoryRegion(size_t size, regionType type);
	void ReleaseMemoryRegion(void* ptr, size_t size);

	void* RequestLargeAllocMemory(size_t size, size_t& commitSize);
	void remove_large_alloc(void* ptr, size_t size);

	void* m_LargeAllocBaseCommitAddress;
	SInt64 m_LargeAllocNextCommitAddress;

	void InitializeMemoryRegion(void* memoryBlock, regionType type, size_t size);
	bool IsTLSFBlock(const void* ptr) const;

	void* CreateTLSFBlock(size_t& blockSize);
	void try_remove_block(void* ptr_in_block);
	void remove_block(void* ptr_in_block);
	void* GetMemoryRegionPointer(const void* ptr) const { return m_LLAlloc->GetMemoryBlockFromPointer(ptr); }
	MemoryRegionInfo* GetMemoryRegionInfo(const void* ptr) const { return (MemoryRegionInfo*)((UInt64)GetMemoryRegionPointer(ptr)); }
	BucketAllocator* m_BucketAllocator;
	tlsf_t m_TlsfInstance;

	void* GetBlockPointer(const void* ptr) const { return (void*)((UInt64)ptr & (~(m_RequestedBlockSize - 1))); }
	MemoryBlockInfo* GetBlockInfo(const void* ptr) const { return (MemoryBlockInfo*)((UInt64)GetBlockPointer(ptr) + m_RequestedBlockSize - sizeof(MemoryBlockInfo)); }
	void* GetTLSFBlockPointer(const void* ptr) const;

	mutable Mutex m_DHAMutex;
	bool m_UseLocking;

	size_t GetControlStructureSize() const;

	UInt32 m_BlocksPerRegion;
	static constexpr size_t kBlockSizeMax = 256LL * 1024 * 1024;
	static constexpr size_t kBlockSizeMin = 128LL * 1024;

	size_t m_RequestedBlockSize;

	LowLevelVirtualAllocator* m_LLAlloc;
	size_t m_LowLevelReservedMemory;

	size_t m_CurrentTLSFBlockCount;
	size_t m_PeakTLSFBlockCount;
	size_t m_CurrentLargeAllocationBytes;
	size_t m_PeakLargeAllocationBytes;
};
