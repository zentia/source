#pragma once

#include "Runtime/Allocator/BaseAllocator.h"
#include "Runtime/Allocator/LowLevelDefaultAllocator.h"

class StackAllocatorBase : public BaseAllocator
{
public:
	StackAllocatorBase(void* blockStart, size_t blockSize, const char* name);
	StackAllocatorBase(void* blockStart, size_t initalBlockSize, size_t commitableBlockSize, const char* name, LowLevelVirtualAllocator* llAlloc);
protected:
	struct Header
	{
		void* prevPtr;
		UInt32 deleted : 1;
		UInt32 size : 31;
	};
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
public:
	void* Allocate(size_t size, int align) override;
	bool Contains(const void* p) const override;
	size_t GetPtrSize(const void* ptr) const override;
private:
	struct Header;

	static inline const Header* GetHeader(const void* ptr);
};

template<AllocatorMode allocMode>
struct StackAllocator<allocMode>::Header : public StackAllocatorBase::Header
{
	inline void** GetCallstack() const { return nullptr; }
	inline int32_t GetCallstackSize() const { return 0; }
};

template <AllocatorMode allocMode>
bool StackAllocator<allocMode>::Contains(const void* p) const
{
	return InBlock(p);
}

template <AllocatorMode allocMode>
size_t StackAllocator<allocMode>::GetPtrSize(const void* ptr) const
{
	return GetHeader(ptr)->size;
}

template <AllocatorMode allocMode>
const typename StackAllocator<allocMode>::Header* StackAllocator<allocMode>::GetHeader(const void* ptr)
{
	return static_cast<const Header*>(ptr) - 1;
}
