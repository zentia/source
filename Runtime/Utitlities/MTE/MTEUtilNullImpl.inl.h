#pragma once
#include "Runtime/Allocator/MemoryMacros.h"

namespace MTEUtil
{
	namespace detail
	{
		static bool Initialize()
		{
			return false;
		}
		template<typename T>
		static constexpr inline size_t RealAddress(T* ptr) { return reinterpret_cast<size_t>(ptr); }
		template<typename T>
		static constexpr inline size_t UntaggedAddress(T* ptr) { return reinterpret_cast<size_t>(ptr); }

		static FORCE_INLINE size_t CopyMemorySizeUnaligned(void* dstData, size_t dstSize, void* srcData, size_t srcSize)
		{
			const size_t copySize = srcSize > dstSize ? dstSize : srcSize;
			SOURCE_MEMCPY(dstData, srcData, copySize);
			return copySize;
		}
	}
}
