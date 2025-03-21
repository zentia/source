#include "SourcePrefix.h"

#include "Runtime/Allocator/MemoryManager.h"

#include <mutex>

#include "DualThreadAllocator.h"
#include "MemoryMacros.h"
#include "MTEAllocator.h"

#include "Runtime/Misc/AllocatorLabels.h"
#include "Allocator/PlatformMemory.h"
#include "Cpp/Algorithm.h"
#include "Runtime/Bootstrap/BootConfig.h"
#include "Runtime/Logging/DebugBreak.h"
#include "Runtime/Logging/LogAssert.h"
#include "Runtime/Misc/SystemInfoMemory.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Utitlities/Argv.h"
#include "Runtime/Utitlities/MTEUtil.h"
#include "Runtime/Allocator/MemorySetup.h"

#define SOURCE_LL_ALLOC(l, s, a) ::_aligned_malloc(s, a)

void* malloc_internal(size_t size, size_t align, MemLabelRef label, AllocateOptions allocateOptions, const char* file, int line)
{
	return GetMemoryManager().Allocate(size, align, label, allocateOptions, file, line);
}

void* operator new(size_t size, MemLabelRef label, size_t align, const char* file, int line)
{
	void* p = malloc_internal(size, align, label, kAllocateOptionNone, file, line);
	return p;
}

#include "Runtime/Allocator/DynamicHeapAllocator.h"
#include "Runtime/Allocator/TLSAllocator.h"
#include "Runtime/Allocator/BucketAllocator.h"
#include "Runtime/Allocator/ThreadsafeLinearAllocator.h"

const size_t kMaxAllocatorOverhead = 64 * 1024;
const char OverflowInMemoryAllocatorString[] = "Overflow in memory allocator.";

typedef DualThreadAllocator<DynamicHeapAllocator> MainThreadAllocator;

#if SOURCE_ALLOC_ALLOW_NEWDELETE_OVERRIDE
void* operator new(size_t size) { return GetMemoryManager().Allocate(size == 0 ? 4 : size, kDefaultMemoryAlignment, kMemNewDelete, kAllocateOptionNone, "Overloaded New"); }
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

struct CreateAllocatorImpl
{
	template<typename T, class ... Args, bool enable = T::IsMTECompliant(), typename std::enable_if<(enable && ENABLE_MEMORY_TAG_EXTENSION), int>::type = 0>
	static T* createAllocator(Args&& ... agrs) { return HEAP_NEW(MTEAllocator<T>(1024 * 1024 * 16), std::forward<Args>(args)...); }

	template<typename T, class ... Args, bool enable = T::IsMTECompliant(), typename std::enable_if<((!enable) || (!ENABLE_MEMORY_TAG_EXTENSION)), int>::type = 0>
	static T* createAllocator(Args&& ... args) { return HEAP_NEW(T) (std::forward<Args>(args)...); }
};

template<typename T, class ... Args>
static T* createAllocator(Args&& ... args) { return CreateAllocatorImpl::createAllocator<T>(std::forward<Args>(args)...); }

MemoryManager* MemoryManager::g_MemoryManager = nullptr;

using namespace memorysetup;

static BootConfig::Parameter<UINT64> s_MainAllocatorBlockSize(kMainAllocatorBlockSizeString, kMainAllocatorBlockSize);
static BootConfig::Parameter<UInt64> s_ThreadAllocatorBlockSize(kThreadAllocatorBlockSizeString, kThreadAllocatorBlockSize);

void* realloc_internal(void* ptr, size_t size, size_t align, MemLabelRef label, AllocateOptions allocateOptions, const char* file, int line)
{
	return GetMemoryManager().Reallocate(ptr, size, align, label, allocateOptions, file, line);
}

void* MemoryManager::Reallocate(void* ptr, size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions, const char* file, int line)
{
	DebugAssert(baselib::Algorithm::IsPowerOfTwo(align));
	DebugAssert(align != 0);
	DebugAssert(GetLabelIdentifier(label) != GetLabelIdentifier(kMemInvalidLabel));

	if (ptr == nullptr)
		return Allocate(size, align, label, allocateOptions, file, line);

	if (size == 0)
	{
		Deallocate(ptr, label, file, line);
		return nullptr;
	}

	align = MaxAlignment(align, kDefaultMemoryAlignment);

	if (baselib::Algorithm::DoesAdditionOverflow(size, align + kMaxAllocatorOverhead))
	{
		WarnAdditionOverflow(allocateOptions);
		return nullptr;
	}

	if (!IsActive())
		ReallocateFallbackToAllocateDeallocate(ptr, size, align, label, allocateOptions, file, line);

	if (IsTempLabel(label))
	{
		return ReallocateFallbackToAllocateDeallocate(ptr, size, align, label, allocateOptions, file, line);
	}

}

void* MemoryManager::ReallocateFallbackToAllocateDeallocate(void* ptr, size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions, const char* file, int line)
{
	void* newptr = Allocate(size, align, label, allocateOptions, file, line);
	if ((allocateOptions & kAllocateOptionReturnNUllIfOutOfMemory) && !newptr)
		return nullptr;

	BaseAllocator* oldAlloc = GetAllocator(label);
	if (!oldAlloc->Contains(ptr))
		oldAlloc = GetAllocatorContainingPtr(ptr);

	size_t oldSize = oldAlloc->GetPtrSize(ptr);
	MTEUtil::CopyMemorySizeUnaligned(newptr, size, ptr, oldSize);

	Deallocate(ptr, label, file, line);
	return newptr;
}


void MemoryManager::WarnAdditionOverflow(AllocateOptions allocateOptions)
{
	if ((allocateOptions & kAllocateOptionReturnNUllIfOutOfMemory) != kAllocateOptionReturnNUllIfOutOfMemory)
	{
		FatalErrorMsg(OverflowInMemoryAllocatorString);
	}
	else
	{
		WarningStringMsg(OverflowInMemoryAllocatorString);
	}
}


MemoryManager::MemoryManager()
	: m_NumAllocators(0)
	, m_IsActive(false)
	, m_FastFrameTempAllocator(nullptr)
	, m_FrameTempAllocator(nullptr)
	, m_BucketAllocator(nullptr)
{
	memset(m_Allocators, 0, sizeof(m_Allocators));
	memset(m_MainAllocators, 0, sizeof(m_MainAllocators));
	memset(m_ThreadAllocators, 0, sizeof(m_ThreadAllocators));

	for (int i = 0; i < kMemLabelCount; i++)
	{
		m_AllocatorMap[i].relatedThreadAllocator = kMemInvalidLabeldId;
		m_AllocatorMap[i].fallbackLabel = kMemInvalidLabeldId;
		m_AllocatorMap[i].fallbackThreadLabel = kMemInvalidLabeldId;
	}
}

void MemoryManager::InitializeFallbackAllocators()
{
	m_InitialFallbackAllocator = createAllocator<DynamicHeapAllocator>(1024 * 1024, true, nullptr, &m_LowLevelAllocator, "ALLOC_FALLBACK", false);

	for (int i = 0; i < kMemLabelCount; i++)
		m_AllocatorMap[i].alloc = m_InitialFallbackAllocator;
}


void MemoryManager::InitializeInitialAllocators()
{
	if (m_FrameTempAllocator == nullptr)
	{
		m_FastFrameTempAllocator = createAllocator<TLSAllocator<> >(&m_LowLevelAllocator, "ALLOC_TEMP_TLS");
		m_FrameTempAllocator = m_FastFrameTempAllocator;
	}

	m_Allocators[m_NumAllocators++] = m_FrameTempAllocator;
	m_AllocatorMap[kMemTempAllocId].alloc = m_FrameTempAllocator;
	m_AllocatorMap[kMemTempAllocId].relatedThreadAllocator = kMemTempJobAllocId;
}

BucketAllocator* MemoryManager::InitializeBucketAllocator()
{
	BucketAllocator* bucketAllocator = nullptr;
	return nullptr;
}

void MemoryManager::InitializeMainThreadAllocator()
{
	if (!m_IsInitializedDebugAllocator)
		InitializeDefaultAllocators();

}


void MemoryManager::InitializeDefaultAllocators()
{
	bool useSystemAllocator = HasARGV("systemallocator");
	bool useDynamicHeapAllocator = !useSystemAllocator;
	bool isLowMemoryPlatform = systeminfo::GetPhysicalMemoryMB() < 2048;

	BucketAllocator* bucketAllocator = useDynamicHeapAllocator ? InitializeBucketAllocator() : nullptr;

	BaseAllocator* defaultAllocator = nullptr;
	if (useDynamicHeapAllocator)
	{
		m_MainAllocators[m_NumAllocators] = createAllocator<DynamicHeapAllocator>(s_MainAllocatorBlockSize, false, nullptr, nullptr, "ALLOC_DEFAULT_MAIN", false);
		m_ThreadAllocators[m_NumAllocators] = createAllocator<DynamicHeapAllocator>(s_ThreadAllocatorBlockSize, true, nullptr, nullptr, "ALLOC_DEFAULT_THREAD", false);
		defaultAllocator = m_Allocators[m_NumAllocators] = createAllocator<MainThreadAllocator>("ALLOC_DEFAULT", bucketAllocator, m_MainAllocators[m_NumAllocators], m_ThreadAllocators[m_NumAllocators], &m_LowLevelAllocator);
		m_AllocatorMap[kMemThreadId].alloc = m_ThreadAllocators[m_NumAllocators];
		m_NumAllocators++;
	}
	else
	{
		
	}
	for (int i = 0; i < kMemLabelCount; i++)
	{
		if (m_AllocatorMap[i].alloc == m_InitialFallbackAllocator)
		{
			m_AllocatorMap[i].alloc = defaultAllocator;
		}
	}
}


namespace
{
	std::once_flag g_InitMemoryManager;
}

void MemoryManager::LateStaticInitialize()
{
	if (GetMemoryManager().m_IsInitialized)
		return;

	GetMemoryManager().InitializeInitialAllocators();

	GetMemoryManager().InitializeMainThreadAllocator();

}


void MemoryManager::InitializeMemoryLazily()
{
	std::call_once(g_InitMemoryManager, []()
		{
			InitializeMemory();
		});
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
	g_MemoryManager->InitializeFallbackAllocators();
}

void* MemoryManager::Allocate(size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions, const char* file, int line)
{
	if (size == 0)
		size = 1;
	BaseAllocator* alloc = GetAllocator(label);
	void* ptr = alloc->Allocate(size, align);
	return ptr;
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

void* MemoryManager::VirtualAllocator::GetMemoryBlockFromPointer(const void* ptrInBlock)
{
	size_t address = MTEUtil::UntaggedAddress(ptrInBlock);
	void* basePtr = (void*)(address & (~(kReserveBlockGranularity - 1)));
	int index = (size_t)basePtr / kReserveBlockGranularity;
	BlockInfo info = GetMemoryBlockOwnerAndOffset(index);
	while (info.offset == 0xff)
	{
		basePtr = (void*)((size_t)basePtr - info.offset * kReserveBlockGranularity);
		index -= 0xff;
		info = GetMemoryBlockOwnerAndOffset(index);
	}
	if (OPTIMIZER_UNLIKELY(info.allocatorIdentifier == 0))
		return nullptr;
	return (void*)((size_t)basePtr - info.offset * kReserveBlockGranularity);
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

BaseAllocator* MemoryManager::GetAllocator(MemLabelRef label)
{
	const MemLabelIdentifier labelId = GetLabelIdentifier(label);
	if (labelId < kMemLabelCount)
	{
		if (m_IsInitializedDebugAllocator)
			return m_Allocators[0];

		BaseAllocator* alloc = m_AllocatorMap[labelId].alloc;
		return alloc;
	}
	else
	{
		Mutex::AutoLock lock(m_CustomAllocatorMutex);

		int index = labelId - kMemLabelCount;
		DebugAssert(index < kMaxCustomAllocators);
		return m_CustomAllocators[index] <= (void*)kMaxCustomAllocators ? nullptr : m_CustomAllocators[index];
	}
}

void* MemoryManager::LowLevelAllocate(size_t size, size_t align)
{
	void* ptr = SOURCE_LL_ALLOC(kMemDefault, size, align);
	if (ptr != nullptr)
		MallocTrackingManager::RegisterLowLevelAlloc(size);
	return ptr;
}
