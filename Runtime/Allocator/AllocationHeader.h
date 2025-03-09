#pragma once

#include "Runtime/Utitlities/MTEUtil.h"

class AllocationSizeHeader
{
public:
	enum
	{
		kHeaderAlignment = 4, // ͷ����Ϣ�����ֽ���
		kMinPaddingSize = 4,
		kPaddingFlag = 1,
		kPaddingMask = ~kPaddingFlag
	};
	size_t GetAllocationSize() const { return m_AllocationSize; }
	uint32_t GetPaddingCount() const { return m_HasPadding ? (*(((const uint32_t*)this) - 1) & kPaddingMask) >> 1 : 0; }
	
	constexpr static size_t GetSize() { return sizeof(AllocationSizeHeader); }
	// ������Ҫ����Ĵ�С
	constexpr static size_t CalculateNeededAllocationSize(size_t size, size_t align)
	{
		return align - 1 + GetSize() + size;
	}
protected:
	size_t m_HasPadding : 1;
	size_t m_AllocationSize : ((sizeof(size_t) << 3) - 1);
};

template<class T>
class AllocationHeaderBase : public T
{
public:
	static const AllocationHeaderBase<T>* GetAllocationHeader(const void* userPtr); // ͷ����Ϣ

	void* GetAllocationPtr() const; // �����ָ��
	size_t AdjustUserPtrSize(size_t size) const; // ����û��ͷ����Ϣ�Ĵ�С
};

template <class T>
size_t AllocationHeaderBase<T>::AdjustUserPtrSize(size_t size) const
{
	return size - T::GetSize() - T::GetPaddingCount();
}


template <class T>
const AllocationHeaderBase<T>* AllocationHeaderBase<T>::GetAllocationHeader(const void* userPtr)
{
	return (AllocationHeaderBase<T>*)MTEUtil::RealAddress((const AllocationHeaderBase<T>*)((const char*)userPtr - T::GetSize()));
}

template <class T>
void* AllocationHeaderBase<T>::GetAllocationPtr() const
{
	return (void*)((const char*)this - T::GetPaddingCount());
}


typedef AllocationHeaderBase<AllocationSizeHeader> AllocationHeader;
typedef AllocationHeaderBase<AllocationSizeHeader> AllocationHeaderWithSize;