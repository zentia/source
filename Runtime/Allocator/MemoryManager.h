#pragma once

#include <atomic>

#include "LowLevelDefaultAllocator.h"
#include "MemoryMacros.h"
#include "TLSAllocator.h"
#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Allocator/MallocTrackingManager.h"
#include "Runtime/Threads/Mutex.h"

class MemoryManager
{
public:
	enum EOperation
	{
		kAllocate = 1 << 0,
		kDeallocate = 1 << 1,
		kReallocate = 1 << 2,
		kFallback = 1 << 3,
		kRollback = 1 << 4,
		kTransfer = 1 << 5,
		kTempMemory = 1 << 6,
		kLeak = 1 << 7,
	};

	static const size_t kMaxAllocators = 16;
	static const size_t kMaxCustomAllocators = 512;

	MemoryManager();
	void InitializeFallbackAllocators();

	static void LateStaticInitialize();
	static void InitializeMemoryLazily();

	static void InitializeMemory();
	bool IsActive() { return m_IsActive; }

	void* Allocate(size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions = kAllocateOptionNone, const char* file = nullptr, int line = 0);
	void* Reallocate(void* ptr, size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions = kAllocateOptionNone, const char* file = nullptr, int line = 0);

	void* ReallocateFallbackToAllocateDeallocate(void* ptr, size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions, const char* file, int line);

	void Deallocate(void* ptr, MemLabelId label, const char* file = nullptr, int line = 0);
	bool TryDeallocateWithLabel(void* ptr, MemLabelId label, const char* file = nullptr, int line = 0);

	void WarnAdditionOverflow(AllocateOptions allocateOptions);

	BaseAllocator* GetAllocatorContainingPtr(const void* ptr);

	struct VirtualAllocator : public LowLevelVirtualAllocator
	{
		VirtualAllocator();

		BaseAllocator* GetAllocatorFromIdentifier(UInt16 identifier);
		BaseAllocator* GetAllocatorFromPointer(const void* ptr);
		virtual BlockInfo GetBlockInfoFromPointer(const void* ptr) override;
		void* GetMemoryBlockFromPointer(const void* ptrInBlock) override;
	private:
		BlockInfo* m_MemoryBlockOwner[kHugeBlockCount];

		static const UInt16 kMaxAllocatorIdentifier = 1 << kAllocatorIdentifierBits;
		BaseAllocator* m_AllocatorsByIdentifier[kMaxAllocatorIdentifier];
		BlockInfo GetMemoryBlockOwnerAndOffset(int index);
	};

	static void* LowLevelAllocate(size_t size, size_t align = kDefaultMemoryAlignment);

	void EarlyDeallocate(void* ptr, MemLabelId label, const char* file, int line);

	BaseAllocator* GetAllocator(MemLabelRef label);

	static MemoryManager* g_MemoryManager;
private:
	void InitializeInitialAllocators();
	void InitializeMainThreadAllocator();
	void InitializeDefaultAllocators();
	class BucketAllocator* InitializeBucketAllocator();

	int m_NumAllocators;
	bool m_IsInitialized;
	bool m_IsActive;
	bool m_IsInitializedDebugAllocator;

	using FrameAllocator_t = TLSAllocatorBase;
	using FastFrameAllocator_t = TLSAllocator<AllocatorMode::Normal>;

	FrameAllocator_t* m_FrameTempAllocator;
	FastFrameAllocator_t* m_FastFrameTempAllocator;
	BaseAllocator* m_BucketAllocator;
	BaseAllocator* m_InitialFallbackAllocator;
	int m_InitialFallbackTempAllocationsCount;

	BaseAllocator* m_Allocators[kMaxAllocators];
	BaseAllocator* m_MainAllocators[kMaxAllocators];
	BaseAllocator* m_ThreadAllocators[kMaxAllocators];

	Mutex m_CustomAllocatorMutex;
	BaseAllocator* m_CustomAllocators[kMaxCustomAllocators];

	struct LabelInfo
	{
		BaseAllocator* alloc;
		MemLabelIdentifier relatedThreadAllocator;
		MemLabelIdentifier fallbackLabel;
		MemLabelIdentifier fallbackThreadLabel;
		alignas(8) std::atomic<size_t> allocatedMemory;
		alignas(8) std::atomic<size_t> largestAlloc;
		alignas(8) std::atomic<size_t> numAllocs;
	};
	LabelInfo m_AllocatorMap[kMemLabelCount];

	VirtualAllocator m_LowLevelAllocator;
};

inline MemoryManager& GetMemoryManager()
{
	if (MemoryManager::g_MemoryManager == nullptr)
	{
		MemoryManager::InitializeMemoryLazily();
	}
	return *MemoryManager::g_MemoryManager;
}