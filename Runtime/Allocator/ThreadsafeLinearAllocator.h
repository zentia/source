#pragma once

#include "SourcePrefix.h"
#include "Runtime/Allocator/BaseAllocator.h"

template<bool EnableTracking>
class alignas(PLATFORM_CACHE_LINE_SIZE)ThreadsafeLinearAllocator : public BaseAllocator
{
public:
	
};
