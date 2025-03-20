#include "SourcePrefix.h"
#include "baselib/Include/Cpp/Algorithm.h"
#include "DynamicHeapAllocator.h"

#include "AllocationHeader.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryManager.h"

#include "External/Allocator/tlsf/tlsf.h"
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
	/*if (newRealPtr)
		GetBlockInfo()*/
	return nullptr;
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


bool DynamicHeapAllocator::IsTLSFBlock(const void* ptr) const
{
	return GetMemoryRegionInfo(ptr)->m_Type == kTLSFBlocks;
}
