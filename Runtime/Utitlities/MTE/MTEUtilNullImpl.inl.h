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
		static constexpr inline size_t UntaggedAddress(T* ptr) { return reinterpret_cast<size_t>(ptr); }
	}
}
