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
	void* m_last_alloc_;
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
	void Deallocate(void* p) override;
	bool TryDeallocate(void* p) override;
	bool Contains(const void* p) const override;
	size_t GetPtrSize(const void* ptr) const override;
	size_t get_requested_ptr_size(const void* ptr) const override;
private:
	struct Header;

	size_t get_overhead_size(void* p) const;

	static inline bool is_deleted(const void* ptr);
	static inline void set_deleted(const void* ptr);
	static inline void* get_prev_alloc(const void* ptr);
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
void* StackAllocator<allocMode>::get_prev_alloc(const void* ptr)
{
	return GetHeader(ptr)->prevPtr;
}

template <AllocatorMode allocMode>
const typename StackAllocator<allocMode>::Header* StackAllocator<allocMode>::GetHeader(const void* ptr)
{
	return static_cast<const Header*>(ptr) - 1;
}

template <AllocatorMode allocMode>
size_t StackAllocator<allocMode>::get_overhead_size(void* p) const
{
	if (void* prev_ptr = get_prev_alloc(p))
	{
		size_t dist_to_last_alloc = (size_t)p - (size_t)prev_ptr;
		return dist_to_last_alloc - GetPtrSize(prev_ptr);
	}
	return (size_t)p - (size_t)m_Block;
}

template <AllocatorMode allocMode>
void StackAllocator<allocMode>::Deallocate(void* p)
{
	if (OPTIMIZER_UNLIKELY(!TryDeallocate(p)))
		WarningStringMsg("Calling Deallocate on pointer, that can not be deallocated by allocator %s", get_name());
}

template <AllocatorMode allocMode>
bool StackAllocator<allocMode>::TryDeallocate(void* p)
{
	if (OPTIMIZER_LIKELY(p == nullptr))
		return true;

	if (p == m_last_alloc_)
	{
		RegisterDeallocationData(StackAllocator<allocMode>::GetPtrSize(p), get_overhead_size(p));
		set_deleted(p);
		do
		{
			m_last_alloc_ = get_prev_alloc(m_last_alloc_);
		}
		while (m_last_alloc_ != nullptr && is_deleted(m_last_alloc_));
		return true;
	}
	if (InBlock(p))
	{
		RegisterDeallocationData(StackAllocator<allocMode>::GetPtrSize(p), get_overhead_size(p));
		set_deleted(p);
		return true;
	}
	return false;
}

template <AllocatorMode allocMode>
size_t StackAllocator<allocMode>::get_requested_ptr_size(const void* ptr) const
{
	return GetPtrSize(ptr);
}

template <AllocatorMode allocMode>
bool StackAllocator<allocMode>::is_deleted(const void* ptr)
{
	return GetHeader(ptr)->deleted != 0;
}

template <AllocatorMode allocMode>
void StackAllocator<allocMode>::set_deleted(const void* ptr)
{
	Header* h = const_cast<Header*>(GetHeader(ptr));
	h->deleted = 1;
}
