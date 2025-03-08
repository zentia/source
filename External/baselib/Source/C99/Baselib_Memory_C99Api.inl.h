#pragma once

#include "Include/C/Baselib_Memory.h"
#include "Include/C/Baselib_Process.h"
#include "Source/Baselib_InlineImplementation.h"
#include "Source/Baselib_Memory_Utils.h"

#include <stdlib.h>
#include <string.h>

namespace C99Api
{
    BASELIB_INLINE_IMPL void* Baselib_Memory_Allocate(size_t size)
    {
        void* allocatedMemory = ::malloc(size);
        if (OPTIMIZER_LIKELY(allocatedMemory != nullptr))
            return allocatedMemory;
        if (size == 0)
            return Baselib_Memory_Allocate(1);

        // Given that we checked for all conditions, a nullptr *should* mean out of memory.
        // Platform specific implementations might be more accurate here.
        ::Baselib_Process_Abort(Baselib_ErrorCode_OutOfMemory);
    }

    BASELIB_INLINE_IMPL void* Baselib_Memory_Reallocate(void* ptr, size_t newSize)
    {
        void* reallocatedMemory = ::realloc(ptr, newSize);
        if (OPTIMIZER_LIKELY(reallocatedMemory != nullptr))
            return reallocatedMemory;
        if (newSize == 0)
            return Baselib_Memory_Allocate(newSize);

        // Given that we checked for all conditions, a nullptr *should* mean out of memory.
        // Platform specific implementations might be more accurate here.
        ::Baselib_Process_Abort(Baselib_ErrorCode_OutOfMemory);
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_Free(void* ptr)
    {
        ::free(ptr);
    }

//
// C99 doesn't have any means of doing aligned allocations,
// and even though posix may have posix_memalign (or memalign)
// those functions are not guaranteed to work with realloc.
// (we do however generally assume that they do, backed up by tests on the particular platforms)
//
// C11/C++17/glibc has aligned_alloc which does work with regular
// realloc. Still doesn't give any guarantees as in terms
// of alignment on realloc and requires to round up to multiples of alignment.
//
}
