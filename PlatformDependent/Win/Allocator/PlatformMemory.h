#pragma once

namespace PlatformMemory
{
	inline void PlatformLowLevelFree(MemLabelRef label, void* ptr)
	{
		::_aligned_free(ptr);
	}

	void Initialize()
	{

	}
}
