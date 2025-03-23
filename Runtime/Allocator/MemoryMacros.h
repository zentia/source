#pragma once

#include "Runtime/Misc/AllocatorLabels.h"
#include "Runtime/Utitlities/NonCopyable.h"
#include <new>

#if defined(_MSC_VER)
#include <malloc.h>
#endif

enum
{
	kDefaultMemoryAlignment = 16
};

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

void* realloc_internal(void* ptr, size_t size, size_t align, MemLabelRef label, AllocateOptions allocateOptions, const char* file, int line);

#define SOURCE_REALLOC(label, ptr, size) realloc_internal(ptr, size, kDefaultMemoryAlignment, label, kAllocateOptionNone, __FILE_STRIPPED__, __LINE__)

inline size_t MaxAlignment(size_t alignment1, size_t alignment2)
{
	return ((alignment1 - 1) | (alignment2 - 1)) + 1;
}

template<typename T>
inline bool DoesAdditionOverflow(T a, T b)
{
	return std::numeric_limits<T>::max() - a < b;
}