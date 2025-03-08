#include "SourcePrefix.h"
#include "baselib/Include/Cpp/Algorithm.h"
#include "DynamicHeapAllocator.h"

#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryManager.h"

#include "External/Allocator/tlsf/tlsf.h"
#include "Runtime/Logging/DebugBreak.h"

DynamicHeapAllocator::DynamicHeapAllocator(UInt32 memoryBlockSize, bool useLocking, BucketAllocator* bucketAllocator, LowLevelVirtualAllocator* llAlloc, const char* name, bool checkEmptyOnDestruct)
	: BaseAllocator(name, !useLocking)
	, m_BucketAllocator(bucketAllocator)
	, m_TlsfInstance(nullptr)
{

}


void* DynamicHeapAllocator::Allocate(size_t size, int align)
{
	DebugAssert(align > 0 && baselib::Algorithm::IsPowerOfTwo(align));

	if (m_BucketAllocator != nullptr && m_BucketAllocator->BucketAllocator::CanAllocate(size, align))
	{
		
	}
}
