#pragma once

#ifndef __cplusplus
#error Baselib_Custom_Allocator.h must never be included from a .c file since that breaks inline namespacing required with multiple baselibs in a project
#endif

/*
    Baselib custom memory allocator - Can be used to override memory allocations internal to Baselib

    BASELIB_CUSTOM_MEMORY_ALLOCATOR should be defined globally in a project to cover all cases where baselib functionality is used.

    Once defined, implement these functions:
        void*   Baselib_Internal_Memory_Allocate(size_t size);
        void*   Baselib_Internal_Memory_Reallocate(size_t size);
        void    Baselib_Internal_Memory_Free(size_t size);
        void*   Baselib_Internal_Memory_AlignedAllocate(size_t size);
        void*   Baselib_Internal_Memory_AlignedReallocate(size_t size);
        void    Baselib_Internal_Memory_AlignedFree(size_t size);
*/
#if defined(BASELIB_CUSTOM_MEMORY_ALLOCATOR)

BASELIB_CPP_INTERFACE
{
    void* Baselib_Internal_Memory_Allocate(size_t size);
    void* Baselib_Internal_Memory_Reallocate(void* ptr, size_t newSize);
    void Baselib_Internal_Memory_Free(void* ptr);
    void* Baselib_Internal_Memory_AlignedAllocate(size_t size, size_t alignment);
    void* Baselib_Internal_Memory_AlignedReallocate(void* ptr, size_t newSize, size_t alignment);
    void Baselib_Internal_Memory_AlignedFree(void* ptr);
}

#else

#include "Include/Baselib.h"
#include "Include/C/Baselib_Memory.h"

BASELIB_CPP_INTERFACE
{
    // Internal paths to allocate memory inside of baselib only
    static inline void* Baselib_Internal_Memory_Allocate(size_t size)
    {
        return Baselib_Memory_Allocate(size);
    }

    static inline void* Baselib_Internal_Memory_Reallocate(void* ptr, size_t newSize)
    {
        return Baselib_Memory_Reallocate(ptr, newSize);
    }

    static inline void Baselib_Internal_Memory_Free(void* ptr)
    {
        return Baselib_Memory_Free(ptr);
    }

    static inline void* Baselib_Internal_Memory_AlignedAllocate(size_t size, size_t alignment)
    {
        return Baselib_Memory_AlignedAllocate(size, alignment);
    }

    static inline void* Baselib_Internal_Memory_AlignedReallocate(void* ptr, size_t newSize, size_t alignment)
    {
        return Baselib_Memory_AlignedReallocate(ptr, newSize, alignment);
    }

    static inline void Baselib_Internal_Memory_AlignedFree(void* ptr)
    {
        return Baselib_Memory_AlignedFree(ptr);
    }
}

#endif
