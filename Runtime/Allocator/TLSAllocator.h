#pragma once
#include "StackAllocator.h"
#include "Runtime/Threads/ThreadSpecificValue.h"

#if ENABLE_MEMORY_MANAGER

#include "Runtime/Allocator/BaseAllocator.h"

class TLSAllocatorBase : public BaseAllocator
{
public:
	
};

template<AllocatorMode allocMode = AllocatorMode::Normal>
class TLSAllocator final : public TLSAllocatorBase
{
public:
	using StackAllocator_t = StackAllocator<allocMode>;

	virtual bool Contains(const void* p) const override;

	StackAllocator_t* GetThreadAllocator() const;
private:
	static SOURCE_TLS_VALUE(StackAllocator_t*) m_UniqueThreadAllocator;

};
#endif
