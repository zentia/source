#pragma once

#include "Runtime/Misc/AllocatorLabels.h"
#include "Runtime/Utitlities/NonCopyable.h"
#include <new>

#if defined(_MSC_VER)
#include <malloc.h>
#endif

enum AllocateOptions
{
	kAllocateOptionNone = 0,
	kAllocateOptionReturnNUllIfOutOfMemory = 1 << 0,
};

#ifndef SOURCE_ALLOC_ALLOW_NEWDELETE_OVERRIDE
#define SOURCE_ALLOC_ALLOW_NEWDELETE_OVERRIDE SOURCE_EDITOR
#endif



#define SOURCE_NEW(type, label) new (label, alignof(type), __FILE_STRIPPED__, __LINE__) type

template<typename T>
inline T constexpr AlignSize(T size, size_t alignment)
{
	return ((size + (alignment - 1)) & ~(alignment - 1));
}

#define SOURCE_MEMCPY memcpy