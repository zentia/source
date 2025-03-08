#pragma once
#include "Runtime/Utitlities/MTEUtil.h"

template<class T>
class MTEAllocator final : public T
{
public:
	template<class ...Args>
	MTEAllocator(const size_t maxTagSize, Args&& ...args) : T(std::forward<Args>(args)...), m_MaxTagSize(maxTagSize) {}
private:
	const size_t m_MaxTagSize;
};