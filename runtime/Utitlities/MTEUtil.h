#pragma once

constexpr auto ENABLE_MEMORY_TAG_EXTENSION = 0;

#include "Runtime/Utitlities/MTE/MTEUtilNullImpl.inl.h"

namespace MTEUtil
{
	static bool Initialize()
	{
		return detail::Initialize();
	}

	template<typename T>
	static constexpr inline size_t RealAddress(T* ptr)
	{
		return detail::RealAddress(ptr);
	}

	template<typename T>
	static constexpr inline size_t UntaggedAddress(T* ptr)
	{
		return detail::UntaggedAddress(ptr);
	}

	static size_t CopyMemorySizeUnaligned(void* dstData, size_t dstSize, void* srcData, size_t srcSize)
	{
		return detail::CopyMemorySizeUnaligned(dstData, dstSize, srcData, srcSize);
	}

	static __forceinline uint32_t MemoryProtectionPageState()
	{
		return detail::MemoryProtectionPageState();
	}
}
