#include "SourcePrefix.h"

#include "Runtime/Allocator/MemoryManager.h"

#include "MemoryMacros.h"

#include "Runtime/Misc/AllocatorLabels.h"
#include "Allocator/PlatformMemory.h"
#include "Runtime/Logging/DebugBreak.h"
#include "Runtime/Logging/LogAssert.h"
#include "Runtime/Utitlities/MTEUtil.h"

void* malloc_internal(size_t size, size_t align, MemLabelRef label, AllocateOptions allocateOptions, const char* file, int line)
{
	
}

void* operator new(size_t size, MemLabelRef label, size_t align, const char* file, int line)
{

}

#if ENABLE_MEMORY_MANAGER
#include "Runtime/Allocator/DynamicHeapAllocator.h"
#include "Runtime/Allocator/TLSAllocator.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/ThreadsafeLinearAllocator.h"

#if SOURCE_ALLOC_ALLOW_NEWDELETE_OVERRIDE
void* operator new(size_t size) { return GetMemoryManager(); }
#endif

#define AlignSize16(size) (AlignSize(size, 16))

const size_t kNumAllocators = 20;
const size_t kAdditionalStaticMem = AlignSize16(sizeof(DynamicHeapAllocator)) * kNumAllocators;

static const int kPreallocatedSize = 
	AlignSize16(sizeof(MemoryManager)) + 
	AlignSize16(sizeof(MallocTrackingManager)) + 
	AlignSize16(sizeof(TLSAllocator<AllocatorMode::TrackLeaks>)) + 
	AlignSize16(sizeof(BucketAllocator)) * 2 +
	AlignSize16(sizeof(ThreadsafeLinearAllocator<true>)) * 4 +
	kAdditionalStaticMem;

alignas(16) static char g_MemoryBlockForMemoryManager[kPreallocatedSize];
static char* g_MemoryBlockPtr = g_MemoryBlockForMemoryManager;

void* GetPreallocatedMemory(int size)
{
	size = AlignSize16(size);

	void* ptr = g_MemoryBlockPtr;
	g_MemoryBlockPtr += size;
	if (g_MemoryBlockPtr > g_MemoryBlockForMemoryManager + kPreallocatedSize)
	{
		return nullptr;
	}
	return ptr;
}

#define HEAP_NEW(TYPE_) new (GetPreallocatedMemory(sizeof(TYPE_))) TYPE_
#define HEAP_DELETE(OBJECT_, TYPE_) OBJECT_->~TYPE_();

MemoryManager* MemoryManager::g_MemoryManager = nullptr;

MemoryManager::MemoryManager()
	: m_NumAllocators(0)
	, m_IsActive(false)
	, m_FastFrameTempAllocator(nullptr)
{
	memset(m_Allocators, 0, sizeof(m_Allocators));
	memset(m_MainAllocators, 0, sizeof(m_MainAllocators));
	memset(m_ThreadAllocators, 0, sizeof(m_ThreadAllocators));
}

void MemoryManager::InitializeMemoryLazily()
{

}

void MemoryManager::InitializeMemory()
{
	if (g_MemoryManager != nullptr)
	{
		return;
	}
#if SOURCE_EDITOR
	PlatformMemory::Initialize();
#endif

	g_MemoryManager = HEAP_NEW(MemoryManager)();
}

void* MemoryManager::Allocate(size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions, const char* file, int line)
{
	
}

void MemoryManager::Deallocate(void* ptr, MemLabelId label, const char* file, int line)
{
	
}

bool MemoryManager::TryDeallocateWithLabel(void* ptr, MemLabelId label, const char* file, int line)
{
	DebugAssert(GetLabelIdentifier(label) != GetLabelIdentifier(kMemInvalidLabel));

	if (ptr == nullptr)
		return true;

	if (!IsActive())
	{
		
	}
}

BaseAllocator* MemoryManager::GetAllocatorContainingPtr(const void* ptr)
{
	BaseAllocator* alloc = m_LowLevelAllocator.GetAllocatorFromPointer(ptr);

	if (alloc)
		return alloc;

	if (OPTIMIZER_UNLIKELY(m_FastFrameTempAllocator != nullptr) && m_FastFrameTempAllocator->Contains(ptr))
		return m_FastFrameTempAllocator;


}

MemoryManager::VirtualAllocator::VirtualAllocator()
{
	memset(m_MemoryBlockOwner, 0, sizeof(m_MemoryBlockOwner));

	for (size_t i = 0; i < kMaxAllocatorIdentifier; i++)
		m_AllocatorsByIdentifier[i] = (BaseAllocator*)(i + 1);
	m_AllocatorsByIdentifier[0] = nullptr;
}

BaseAllocator* MemoryManager::VirtualAllocator::GetAllocatorFromIdentifier(UInt16 identifier)
{
	if ((size_t)m_AllocatorsByIdentifier[identifier] < kMaxAllocatorIdentifier)
		return nullptr;
	return m_AllocatorsByIdentifier[identifier];
}

BaseAllocator* MemoryManager::VirtualAllocator::GetAllocatorFromPointer(const void* ptr)
{
	return GetAllocatorFromIdentifier(VirtualAllocator::GetBlockInfoFromPointer(ptr).allocatorIdentifier);
}

LowLevelVirtualAllocator::BlockInfo MemoryManager::VirtualAllocator::GetBlockInfoFromPointer(const void* ptr)
{
	size_t address = MTEUtil::UntaggedAddress(ptr);
	int blockIndex = address / kReserveBlockGranularity;
	return GetMemoryBlockOwnerAndOffset(blockIndex);
}

LowLevelVirtualAllocator::BlockInfo MemoryManager::VirtualAllocator::GetMemoryBlockOwnerAndOffset(int index)
{
	UInt32 hugeBlockIndex = index / kReserveBlockCount;
	UInt32 blockIndex = index % kReserveBlockCount;
	if (OPTIMIZER_UNLIKELY(m_MemoryBlockOwner[hugeBlockIndex] == nullptr))
		return BlockInfo();

	return m_MemoryBlockOwner[hugeBlockIndex][blockIndex];
}


void MemoryManager::EarlyDeallocate(void* ptr, MemLabelId label, const char* file, int line)
{
	
}


#endif

