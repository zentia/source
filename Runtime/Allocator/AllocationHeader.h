#pragma once

#include "Runtime/Utitlities/MTEUtil.h"

class AllocationSizeHeader
{
public:
	enum
	{
		kHeaderAlignment = 4, // 头部信息对其字节数
		kMinPaddingSize = 4,
		kPaddingFlag = 1,
		kPaddingMask = ~kPaddingFlag
	};
	// Padding of allocation header have few nuances:
	// kPadValue is not random: its binary interpretation (0x10101011) still have 1st bit set.
	static constexpr UInt8 kPadValue = 0xAB;
	static const UInt32 kPadValue4Bytes = (kPadValue << 24) | (kPadValue << 16) | (kPadValue << 8) | kPadValue;
	void InitAllocationHeader(void* allocPtr, size_t padCount, size_t size, int32_t allocatorId)
	{
		SetPadding(allocPtr, padCount);
		m_AllocationSize = size;
	}
	void InitAllocationFooter(void* footerPtr) {}
	size_t GetAllocationSize() const { return m_AllocationSize; }
	uint32_t GetPaddingCount() const { return m_HasPadding ? (*(((const uint32_t*)this) - 1) & kPaddingMask) >> 1 : 0; }

	constexpr static size_t GetSelfSize() { return sizeof(AllocationSizeHeader); }
	constexpr static size_t GetSize() { return sizeof(AllocationSizeHeader); }
	constexpr static size_t GetOverheadSize() { return GetSelfSize() + kDefaultMemoryAlignment - 1; }
	// 计算需要分配的大小
	constexpr static size_t CalculateNeededAllocationSize(size_t size, size_t align)
	{
		return align - 1 + GetSize() + size;
	}
	static size_t GetRequiredPadding(void* allocPtr, size_t userAlign)
	{
		return userAlign - ((((size_t)allocPtr + GetSize() - 1) & (userAlign - 1)) + 1);
	}
protected:
	size_t m_HasPadding : 1;
	size_t m_AllocationSize : ((sizeof(size_t) << 3) - 1);
private:
	void SetPadding(void* allocPtr, size_t padCount)
	{
		m_HasPadding = (padCount > 0);
		if (padCount > 0)
		{
			if (padCount > sizeof(uint32_t))
				memset(allocPtr, kPadValue, padCount - sizeof(uint32_t));

			uint32_t* paddingPtr = ((uint32_t*)this) - 1;

			AssertMsg((padCount & kPaddingFlag) == 0, "Padding count cannot use 31 bit");

			*paddingPtr = (padCount << 1) | kPaddingFlag;
		}
	}
};

template<class T>
class AllocationHeaderBase : public T
{
public:

	static AllocationHeaderBase<T>* Init(void* allocPtr, int allocatorId, size_t size, size_t align); // init header data
	static const AllocationHeaderBase<T>* GetAllocationHeader(const void* userPtr); // 头部信息

	void* GetUserPtr() const; 

	void* GetAllocationPtr() const; // 分配的指针
	size_t AdjustUserPtrSize(size_t size) const; // 返回没有头部信息的大小
};

template <class T>
AllocationHeaderBase<T>* AllocationHeaderBase<T>::Init(void* allocPtr, int allocatorId, size_t size, size_t align)
{
	static_assert(sizeof(AllocationHeaderBase<T>) == sizeof(T), "AllocationHeaderBase don't add any overhead on top of base class");

	size_t padCount = T::GetRequiredPadding(allocPtr, align);
	AllocationHeaderBase<T>* header = (AllocationHeaderBase<T>*)((char*)allocPtr + padCount);
	header->InitAllocationHeader(allocPtr, padCount, size, allocatorId);
	header->InitAllocationFooter(((char*)header->GetUserPtr()) + size);
	return header;
}

template <class T>
void* AllocationHeaderBase<T>::GetUserPtr() const
{
	return (void*)((const char*)this + T::GetSize());
}

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