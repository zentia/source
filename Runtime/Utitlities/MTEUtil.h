#pragma once

#include "Runtime/Utitlities/MTE/MTEUtilNullImpl.inl.h"

namespace MTEUtil
{
	static bool Initialize()
	{
		return detail::Initialize();
	}

	template<typename T>
	static constexpr inline size_t UntaggedAddress(T* ptr)
	{
		return detail::UntaggedAddress(ptr);
	}
}
