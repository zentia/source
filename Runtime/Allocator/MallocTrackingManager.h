#pragma once
#include "Cpp/Atomic.h"

#if ENABLE_MEMORY_MANAGER

#include "BaseAllocator.h"

class MallocTrackingManager : public BaseAllocator
{
public:
	static void RegisterLowLevelAlloc(size_t size) { s_MallocLLAllocBytes += size; }
	static void RegisterLowLevelFree(size_t size) { s_MallocLLAllocBytes -= size; }
private:
	static baselib::atomic<size_t> s_MallocLLAllocBytes;
};
#endif
