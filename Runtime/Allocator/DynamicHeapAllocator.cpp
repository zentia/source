#include "SourcePrefix.h"
#include "baselib/Include/Cpp/Algorithm.h"
#include "DynamicHeapAllocator.h"

#include "AllocationHeader.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryManager.h"

#include "External/tlsf/tlsf.h"
#include "Runtime/Core/Containers/Format/Format.h"
#include "Runtime/Logging/DebugBreak.h"
#include "Runtime/Utitlities/BitUtility.h"

static size_t GetTlsfAllocationSize(const AllocationHeader* header)
{
	size_t size = tlsf_block_size(header->GetAllocationPtr());
	return header->AdjustUserPtrSize(size);
}

DynamicHeapAllocator::DynamicHeapAllocator(UInt32 memoryBlockSize, bool useLocking, BucketAllocator* bucketAllocator, LowLevelVirtualAllocator* llAlloc, const char* name, bool checkEmptyOnDestruct)
	: BaseAllocator(name, !useLocking)
	, m_BucketAllocator(bucketAllocator)
	, m_TlsfInstance(nullptr)
	, m_UseLocking(useLocking)
	, m_LLAlloc(llAlloc)
{
	AssertMsg(memoryBlockSize <= kBlockSizeMax && memoryBlockSize >= kBlockSizeMin, core::Format("Allocator"));
	InitializeTLSF();
}

void DynamicHeapAllocator::SetLowLevelAllocator(LowLevelVirtualAllocator* llAlloc)
{
	Assert(llAlloc && !m_LLAlloc);
	m_LLAlloc = llAlloc;
}

void DynamicHeapAllocator::InitializeTLSF()
{
	m_TlsfInstance = tlsf_create(GetMemoryManager().LowLevelAllocate(tlsf_size(), tlsf_align_size()));
}

size_t DynamicHeapAllocator::GetControlStructureSize() const
{
	return AlignSize(sizeof(MemoryRegionInfo) + sizeof(SInt16) * m_BlocksPerRegion, 16);
}

void* DynamicHeapAllocator::Allocate(size_t size, int align)
{
	DebugAssert(align > 0 && baselib::Algorithm::IsPowerOfTwo(align));

	// 64字节以下的内存申请使用无锁的块分配器
	if (m_BucketAllocator != nullptr && m_BucketAllocator->BucketAllocator::CanAllocate(size, align))
	{
		void* realPtr = m_BucketAllocator->BucketAllocator::Allocate(size, align);
		if (realPtr != nullptr)
			return realPtr;
	}

	bool alignmentInHeader = AllocationHeader::GetSize() != 0;
	int allocationAlignment = alignmentInHeader ? kDefaultMemoryAlignment : align;

	size_t realSize = AllocationHeader::CalculateNeededAllocationSize(size, align);

	// 对齐到tlsf要求的尺寸大小
	if (realSize > 32)
	{
		size_t tlsfalign = (1 << HighestBit(realSize >> 5)) - 1;

		if (baselib::Algorithm::DoesAdditionOverflow(realSize, tlsfalign))
		{
			FatalErrorMsg("Size overflow in allocator.");
		}
	}

	char* newRealPtr = nullptr;

	if (m_UseLocking)
		m_DHAMutex.Lock();

	newRealPtr = (char*)tlsf_memalign(m_TlsfInstance, allocationAlignment, realSize);
	if (newRealPtr)
		GetBlockInfo(newRealPtr)->allocationCount += 1;

	if (m_UseLocking)
		m_DHAMutex.Unlock();

	if (newRealPtr == nullptr)
	{
		if (size < m_RequestedBlockSize / 4)
		{
			if (m_UseLocking)
				m_DHAMutex.Lock();

			size_t tlsfBlockSize;
			void* memoryBlock = CreateTLSFBlock(tlsfBlockSize);

			tlsf_add_pool(m_TlsfInstance, memoryBlock, tlsfBlockSize);

			newRealPtr = (char*)tlsf_memalign(m_TlsfInstance, allocationAlignment, realSize);
			if (newRealPtr)
				GetBlockInfo(newRealPtr)->allocationCount += 1;

			if (m_UseLocking)
				m_DHAMutex.Unlock();

			Assert(newRealPtr);
		}

		if (newRealPtr == nullptr)
		{
			// Large alloc uses AllocationHeaderWithSize header
			realSize = AllocationHeaderWithSize::CalculateNeededAllocationSize(size + sizeof(LargeAllocInfo), align);
			realSize = AlignSize(realSize, m_LLAlloc->GetPageSize());

			if (m_UseLocking)
				m_DHAMutex.Lock();

			size_t commitSize;
			void* largeAllocPtr = RequestLargeAllocMemory(realSize, commitSize);
			if (largeAllocPtr == nullptr)
			{
				if (m_UseLocking)
					m_DHAMutex.Unlock();

				return nullptr;
			}

			LargeAllocInfo* largeAllocNode = (LargeAllocInfo*)((UInt64)largeAllocPtr);
			new(largeAllocNode) LargeAllocInfo();

			largeAllocNode->allocatedSize = commitSize;
			MemoryRegionInfo* regionInfo = GetMemoryRegionInfo(largeAllocPtr);
			regionInfo->m_LargeAllocs.push_back(*largeAllocNode);
			regionInfo->m_LargeAllocCount++;

			if (m_UseLocking)
				m_DHAMutex.Unlock();

			newRealPtr = (char*)(largeAllocNode + 1);
		}
	}

	void* ptr;
	if (IsTLSFBlock(newRealPtr))
	{
		AllocationHeader* newAllocHeader = AllocationHeader::Init(newRealPtr, m_AllocatorIdentifier, size, align);
		ptr = newAllocHeader->GetUserPtr();

		RegisterAllocationData(GetTlsfAllocationSize(newAllocHeader), newAllocHeader->GetOverheadSize());
	}
	else
	{
		AllocationHeaderWithSize* newAllocHeader = AllocationHeaderWithSize::Init(newRealPtr, m_AllocatorIdentifier, size, align);
		ptr = newAllocHeader->GetUserPtr();

		RegisterAllocationData(size, realSize - size);
	}

	return ptr;
}

void DynamicHeapAllocator::Deallocate(void* ptr)
{
	if (ptr == nullptr)
		return;

	if (m_BucketAllocator != nullptr && m_BucketAllocator->BucketAllocator::TryDeallocate(ptr))
		return;
	if (IsTLSFBlock(ptr))
	{
		const AllocationHeader* allocHeader = AllocationHeader::GetAllocationHeader(ptr);
		void* realPtr = allocHeader->GetAllocationPtr();
	}
}

bool DynamicHeapAllocator::Contains(const void* p) const
{
	return true;
}

size_t DynamicHeapAllocator::GetPtrSize(const void* ptr) const
{
	size_t size = 0;
	if (m_BucketAllocator != nullptr)
	{
		size = m_BucketAllocator->BucketAllocator::GetPtrSize(ptr);
		if (size != 0)
			return size;
	}

	if (IsTLSFBlock(ptr))
	{
		// tlsf allocation
		const AllocationHeader* allocHeader = AllocationHeader::GetAllocationHeader(ptr);
		size = GetTlsfAllocationSize(allocHeader);
		return size;
	}

	// large allocation
	const AllocationHeaderWithSize* largeAllocHeader = AllocationHeaderWithSize::GetAllocationHeader(ptr);
	size = largeAllocHeader->GetAllocationSize();

	return size;
}

void DynamicHeapAllocator::ReleaseMemoryBlock(void* ptr, size_t size)
{
	m_LLAlloc->ReleaseMemoryBlock(ptr, size);
	m_LowLevelReservedMemory -= size;
}

void* DynamicHeapAllocator::ReserveMemoryRegion(size_t size, regionType type)
{
	LowLevelVirtualAllocator::BlockInfo info;
	info.allocatorIdentifier = m_AllocatorIdentifier;
	info.blockType = type;
	m_LowLevelReservedMemory += size;
	return m_LLAlloc->ReserveMemoryBlock(size, info);
}

void DynamicHeapAllocator::ReleaseMemoryRegion(void* ptr, size_t size)
{
	ReleaseMemoryBlock(ptr, size);
}

void* DynamicHeapAllocator::RequestLargeAllocMemory(size_t size, size_t& commitSize)
{
	if (m_LargeAllocBaseCommitAddress == nullptr || m_LargeAllocNextCommitAddress + size > reinterpret_cast<UInt64>(m_LargeAllocBaseCommitAddress) + GetMemoryRegionInfo(m_LargeAllocBaseCommitAddress)->m_RegionSize)
	{
		size_t pageSize = m_LLAlloc->GetPageSize();
		// in the case of size is multiple of kReserveBlockGranularity, we reserve memory for the size only,
		// and then commit the MemoryRegionInfo first, and for the actual commit we wont have enough memory.
		// e.g. if size if equal to 268435456
		size_t requiredSize = AlignSize(AlignSize(size, pageSize) + AlignSize(sizeof(MemoryRegionInfo), pageSize), LowLevelVirtualAllocator::kReserveBlockGranularity);
		m_LargeAllocBaseCommitAddress = ReserveMemoryRegion(requiredSize, kLargeAlloc);

		if (m_LargeAllocBaseCommitAddress == nullptr)
			return nullptr;

		size_t commitSize = m_LLAlloc->CommitMemory(m_LargeAllocBaseCommitAddress, sizeof(MemoryRegionInfo));

		if (commitSize == 0)
		{
			ReleaseMemoryRegion(m_LargeAllocBaseCommitAddress, requiredSize);
			m_LargeAllocBaseCommitAddress = nullptr;
			return nullptr;
		}
		m_TotalReservedBytes += commitSize;

		InitializeMemoryRegion(m_LargeAllocBaseCommitAddress, kLargeAlloc, requiredSize);
		m_LargeAllocNextCommitAddress = reinterpret_cast<UInt64>(m_LargeAllocBaseCommitAddress) + commitSize;
	}

	void* largeAllocPtr = reinterpret_cast<void*>(m_LargeAllocNextCommitAddress);
	m_LargeAllocNextCommitAddress += size;

	commitSize = m_LLAlloc->CommitMemory(largeAllocPtr, size);
	if (commitSize == 0)
		return nullptr;

	m_CurrentLargeAllocationBytes += commitSize;
	m_PeakLargeAllocationBytes = std::max(m_PeakLargeAllocationBytes, m_CurrentLargeAllocationBytes);
	m_TotalAllocatedBytes += commitSize;
	return largeAllocPtr;
}

void DynamicHeapAllocator::InitializeMemoryRegion(void* memoryBlock, regionType type, size_t size)
{
	MemoryRegionInfo* regionInfo = GetMemoryRegionInfo(memoryBlock);
	new(regionInfo) MemoryRegionInfo();
	if (type == kTLSFBlocks)
	{
		// initialize freelist, but remove item at index 0 for immediate use
		regionInfo->m_MemoryBlockFreeList = (SInt16*)(regionInfo + 1);
		regionInfo->m_NextFreeMemoryBlockIndex = 1;
		regionInfo->m_MemoryBlockFreeList[0] = -1;
		for (int i = 1; i < m_BlocksPerRegion; i++)
			regionInfo->m_MemoryBlockFreeList[i] = i + 1;
		regionInfo->m_MemoryBlockFreeList[m_BlocksPerRegion - 1] = -1;
	}
	regionInfo->m_LargeAllocCount = 0;
	regionInfo->m_Type = type;
	regionInfo->m_RegionSize = size;
	m_MemoryRegions.push_back(*regionInfo);
}

void* DynamicHeapAllocator::GetTLSFBlockPointer(const void* ptr) const
{
	void* blockPointer = GetBlockPointer(ptr);
	// if the block is the first in the region, there is a ControlStructure in front
	if (blockPointer == GetMemoryRegionPointer(ptr))
		return (void*)(((UInt64)blockPointer) + GetControlStructureSize());
	return blockPointer;
}

bool DynamicHeapAllocator::IsTLSFBlock(const void* ptr) const
{
	return GetMemoryRegionInfo(ptr)->m_Type == kTLSFBlocks;
}

void* DynamicHeapAllocator::CreateTLSFBlock(size_t& tlsfBlockSize)
{
	// Get free MemoryBlockIndex in Region
	size_t controlStructureSize = sizeof(MemoryBlockInfo);
	void* block = nullptr;
	size_t blockSize = 0;
	void* baseAddress = nullptr;
	for (ListIterator<MemoryRegionInfo> iMR = m_MemoryRegions.begin(); iMR != m_MemoryRegions.end(); iMR++)
	{
		if ((*iMR).m_Type == kTLSFBlocks)
		{
			SInt16 blockIndex = (*iMR).m_NextFreeMemoryBlockIndex;
			if (blockIndex != -1)
			{
				(*iMR).m_NextFreeMemoryBlockIndex = (*iMR).m_MemoryBlockFreeList[blockIndex];
				(*iMR).m_MemoryBlockFreeList[blockIndex] = -1;
				baseAddress = (void*)((UInt64)(GetMemoryRegionPointer(&(*iMR))) + blockIndex * m_RequestedBlockSize);
				break;
			}
		}
	}
	if (baseAddress == nullptr)
	{
		// if no free blocks are left in the region, allocate block in new region
		block = ReserveMemoryRegion(m_RequestedBlockSize, kTLSFBlocks);
		controlStructureSize += GetControlStructureSize();

		blockSize = m_LLAlloc->CommitMemory(block, m_RequestedBlockSize);
		InitializeMemoryRegion(block, kTLSFBlocks, m_RequestedBlockSize);
	}
	else
	{
		blockSize = m_LLAlloc->CommitMemory(baseAddress, m_RequestedBlockSize);
		block = baseAddress;
	}
	m_TotalReservedBytes += blockSize;
	m_CurrentTLSFBlockCount++;
	m_PeakTLSFBlockCount = std::max(m_PeakTLSFBlockCount, m_CurrentTLSFBlockCount);

	MemoryBlockInfo* blockInfo = GetBlockInfo(block);
	new(blockInfo) MemoryBlockInfo();
	blockInfo->allocationCount = 0;

	blockInfo->canDecommitBlock = GetBlockPointer(block) != GetMemoryRegionPointer(block);

	MemoryRegionInfo* regionInfo = GetMemoryRegionInfo(block);
	regionInfo->m_TLSFMemoryBlocks.push_back(*blockInfo);

	tlsfBlockSize = blockSize - controlStructureSize;
	return GetTLSFBlockPointer(block);
}
