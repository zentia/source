#pragma once

#include <atomic>

#include "LowLevelDefaultAllocator.h"
#include "MemoryMacros.h"
#include "TLSAllocator.h"
#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Allocator/MallocTrackingManager.h"
#include "Runtime/Threads/Mutex.h"
#include "Runtime/Threads/ReadWriteLock.h"

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
	void Deallocate(void* ptr, const char* file = nullptr, int line = 0);
	void FallbackDeallocate(void* ptr, MemLabelRef label, const char* file, int line);
	bool TryDeallocate(void* ptr, const char* file = nullptr, int line = 0);
	bool TryDeallocateWithLabel(void* ptr, MemLabelId label, const char* file = nullptr, int line = 0);

	void WarnAdditionOverflow(AllocateOptions allocateOptions);

	uint16_t RegisterAllocator(BaseAllocator* alloc);

	BaseAllocator* GetAllocatorContainingPtr(const void* ptr);

	MemLabelId GetFallbackLabel(MemLabelId label);

	struct VirtualAllocator : public LowLevelVirtualAllocator
	{
		VirtualAllocator();

		void* ReserveMemoryBlock(size_t size, BlockInfo info) override;
		void ReleaseMemoryBlock(void* ptr, size_t size) override;
		size_t CommitMemory(void* ptr, size_t size) override;
		size_t DecommitMemory(void* ptr, size_t size) override;

		uint16_t RegisterAllocator(BaseAllocator* alloc);
		BaseAllocator* GetAllocatorFromIdentifier(UInt16 identifier);
		BaseAllocator* GetAllocatorFromPointer(const void* ptr);
		virtual BlockInfo GetBlockInfoFromPointer(const void* ptr) override;
		void* GetMemoryBlockFromPointer(const void* ptrInBlock) override;

		void MarkMemoryBlocks(void* returnAddress, size_t size, BlockInfo info);
	private:
		Mutex m_LowLevelAllocLock;
		BlockInfo* m_MemoryBlockOwner[kHugeBlockCount];

		static constexpr UInt16 kMaxAllocatorIdentifier = 1 << kAllocatorIdentifierBits;
		uint16_t m_NextFreeIdentifier;
		BaseAllocator* m_AllocatorsByIdentifier[kMaxAllocatorIdentifier];
		void SetMemoryBlockOwnerAndOffset(int index, BlockInfo info);
		BlockInfo GetMemoryBlockOwnerAndOffset(int index);

		baselib::atomic<size_t> m_LowLevelReservedMemory;
		baselib::atomic<size_t> m_LowLevelCommittedMemory;

		ReadWriteLock m_MemoryApiExclusiveAccess;
		inline ReadWriteLock& MemoryApiExclusiveAccessLock() { return m_MemoryApiExclusiveAccess; }
	};

	static void* LowLevelAllocate(size_t size, size_t align = kDefaultMemoryAlignment);
	static void LowLevelFree(void* p, size_t oldSize);

	static inline bool IsTempAllocatorLabel(MemLabelRef label) { return GetLabelIdentifier(label) == kMemTempAllocId; }

	void* EarlyAllocate(size_t size, size_t align, MemLabelId label, const char* file, int line);

	// Handle any deallocation before memory manager is fully active
	void EarlyDeallocate(void* ptr, MemLabelId label, const char* file, int line);

	BaseAllocator* GetAllocator(MemLabelRef label);

	struct allocation_log_details
	{
		allocation_log_details(void* ptr, const size_t size, const size_t align, const MemLabelId label, const char* function, const char* file, const int line)
			: ptr(ptr)
			, size(size)
			, align(align)
			, label(label)
			, function(function)
			, file(file)
			, line(line)
		{}
		void* ptr;
		size_t size;
		size_t align;
		MemLabelId label;
		const char* function;
		const char* file;
		int line;
	};
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
	MemLabelIdentifier m_CustomAllocatorFallbacks[kMaxCustomAllocators];

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