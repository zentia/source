#pragma once

#include <atomic>

#include "LowLevelDefaultAllocator.h"
#include "MemoryMacros.h"
#include "TLSAllocator.h"
#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Allocator/MallocTrackingManager.h"

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

	static void InitializeMemoryLazily();

	static void InitializeMemory();
	bool IsActive() { return m_IsActive; }

	void* Allocate(size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions = kAllocateOptionNone, const char* file = nullptr, int line = 0);
	void* Reallocate(void* ptr, size_t size, size_t align, MemLabelId label, AllocateOptions allocateOptions = kAllocateOptionNone, const char* file = nullptr, int line = 0);

	void Deallocate(void* ptr, MemLabelId label, const char* file = nullptr, int line = 0);
	bool TryDeallocateWithLabel(void* ptr, MemLabelId label, const char* file = nullptr, int line = 0);

	BaseAllocator* GetAllocatorContainingPtr(const void* ptr);

	struct VirtualAllocator : public LowLevelVirtualAllocator
	{
		VirtualAllocator();

		BaseAllocator* GetAllocatorFromIdentifier(UInt16 identifier);
		BaseAllocator* GetAllocatorFromPointer(const void* ptr);
		virtual BlockInfo GetBlockInfoFromPointer(const void* ptr) override;
	private:
		BlockInfo* m_MemoryBlockOwner[kHugeBlockCount];

		static const UInt16 kMaxAllocatorIdentifier = 1 << kAllocatorIdentifierBits;
		BaseAllocator* m_AllocatorsByIdentifier[kMaxAllocatorIdentifier];
		BlockInfo GetMemoryBlockOwnerAndOffset(int index);
	};

	void EarlyDeallocate(void* ptr, MemLabelId label, const char* file, int line);

	static MemoryManager* g_MemoryManager;
private:
	int m_NumAllocators;
	using FastFrameAllocator_t = TLSAllocator<AllocatorMode::Normal>;

	FastFrameAllocator_t* m_FastFrameTempAllocator;

	BaseAllocator* m_Allocators[kMaxAllocators];
	BaseAllocator* m_MainAllocators[kMaxAllocators];
	BaseAllocator* m_ThreadAllocators[kMaxAllocators];

	bool m_IsActive;

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
		
	}
	return *MemoryManager::g_MemoryManager;
}