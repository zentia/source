#pragma once
#include "StackAllocator.h"
#include "Runtime/Threads/ThreadSpecificValue.h"

#if ENABLE_MEMORY_MANAGER

#include "Runtime/Allocator/BaseAllocator.h"

class TLSAllocatorBase : public BaseAllocator
{
public:
	TLSAllocatorBase(const char* name, bool isThreadSafe = false);
};

template<AllocatorMode allocMode = AllocatorMode::Normal>
class TLSAllocator final : public TLSAllocatorBase
{
public:
	using StackAllocator_t = StackAllocator<allocMode>;

	TLSAllocator(LowLevelVirtualAllocator* llAlloc, const char* name);

	void* Allocate(size_t size, int align) override;
	virtual bool Contains(const void* p) const override;
	void Deallocate(void* p) override;
	size_t GetPtrSize(const void* ptr) const override;

	static constexpr bool IsMTECompliant() { return true; }

	StackAllocator_t* GetThreadAllocator() const;
private:
	static SOURCE_TLS_VALUE(StackAllocator_t*) m_UniqueThreadAllocator;

};
#endif
