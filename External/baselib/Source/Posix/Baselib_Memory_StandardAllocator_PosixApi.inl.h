#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "Include/C/Baselib_Memory.h"
#include "Include/C/Baselib_Process.h"
#include "Source/Baselib_Memory_Utils.h"

#include <string.h>
#include <stdlib.h>
#include <errno.h>

namespace PosixApi
{
    BASELIB_INLINE_IMPL void* Baselib_Memory_AlignedAllocate(size_t size, size_t alignment)
    {
        // posix_memalign is allowed to fail for zero size
        //
        // https://pubs.opengroup.org/onlinepubs/9699919799/functions/posix_memalign.html
        // If size is 0, either:
        //  posix_memalign() shall not attempt to allocate any space, in which case either an implementation-defined error number
        //  shall be returned, or zero shall be returned with a null pointer returned in memptr, or
        //  posix_memalign() shall attempt to allocate some space and, if the allocation succeeds, zero shall be returned and
        //  a pointer to the allocated space shall be returned in memptr. The application shall ensure that the pointer is not used to access an object.
        //
        if (size == 0)
            size = 1;

        void* allocatedMemory = nullptr;
        int result = ::posix_memalign(&allocatedMemory, alignment, size);
        BaselibAssert(result != EINVAL, "Invalid argument was passed to posix_memalign, this should not be possible since Baselib's argument validation should have caught this");

        if (result == ENOMEM)
        {
            Baselib_Process_Abort(Baselib_ErrorCode_OutOfMemory);
            return nullptr;
        }
        return allocatedMemory;
    }

    BASELIB_INLINE_IMPL void* Baselib_Memory_AlignedReallocate(void* ptr, size_t newSize, size_t alignment)
    {
        if (ptr == nullptr)
            return Baselib_Memory_AlignedAllocate(newSize, alignment);

        // WARNING: It is not generally specified if realloc works with posix_memalign!
        // On some platforms this may crash, but it has been proven to be workable on others.
        ptr = ::realloc(ptr, newSize);
        if (ptr == nullptr)
        {
            if (newSize == 0)
                return Baselib_Memory_AlignedAllocate(newSize, alignment);

            // Given that we checked for all conditions, a nullptr *should* mean out of memory.
            // Platform specific implementations might be more accurate here.
            Baselib_Process_Abort(Baselib_ErrorCode_OutOfMemory);
            return nullptr;
        }

        // There is no standardized way to reallocate with alignment.
        // Also, realloc does not give any guarantees on alignment.
        if (IsAligned(ptr, alignment))
            return ptr;

        void* newPtr = Baselib_Memory_AlignedAllocate(newSize, alignment);
        memcpy(newPtr, ptr, newSize);
        Baselib_Memory_AlignedFree(ptr);
        return newPtr;
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_AlignedFree(void* ptr)
    {
        free(ptr);
    }
}
