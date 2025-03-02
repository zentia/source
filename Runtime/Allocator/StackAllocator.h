#pragma once

#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"

class StackAllocatorBase : public BaseAllocator
{
public:
	StackAllocatorBase(void* blockStart, size_t blockSize, const char* name);
	StackAllocatorBase(void* blockStart, size_t initalBlockSize, size_t commitableBlockSize, const char* name, LowLevelVirtualAllocator* llAlloc);
protected:
	void* m_Block;
	size_t m_CurrentSize;

	bool InBlock(const void* ptr) const;
};

inline bool StackAllocatorBase::InBlock(const void* ptr) const
{
	return ptr >= m_Block && ptr < (static_cast<char*>(m_Block) + m_CurrentSize);
}

template<AllocatorMode allocMode = AllocatorMode::Normal>
class StackAllocator : public StackAllocatorBase
{
	bool Contains(const void* p) const override;
};

template <AllocatorMode allocMode>
bool StackAllocator<allocMode>::Contains(const void* p) const
{
	return InBlock(p);
}
