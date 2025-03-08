#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "../Posix/Baselib_Memory_PosixApi.inl.h"

namespace LinuxApi
{
namespace detail
{
    // Note that the meaning for EINVAL/ENOMEM is different for madvise/mprotect compared with other related apis.
    // (also, on Darwin based systems different errors are returned!)
    static void RaiseError_madvise_mprotect(Baselib_ErrorState* errorState, const Baselib_SourceLocation& sourceLocation)
    {
        const int errnoErrorCode = errno;
        switch (errnoErrorCode)
        {
            case ENOMEM:
                errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_InvalidAddressRange, sourceLocation) | WithErrno(errnoErrorCode);
                return;
            case EACCES:
                errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnsupportedPageState, sourceLocation) | WithErrno(errnoErrorCode);
                return;
        }
        BaselibAssert(false, "madvise or mprotect call failed with unexpected error: %d", errnoErrorCode);
        errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnexpectedError, sourceLocation) | WithErrno(errnoErrorCode);
    }
}

    BASELIB_INLINE_IMPL void Baselib_Memory_SetPageStateEx(void* addressOfFirstPage, uint64_t pageSize, uint64_t pageCount, Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        int protection = static_cast<int>(PosixApi::detail::PageStateToNativeState(static_cast<Baselib_Memory_PageState>(pageState), extPageState, errorState));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;

        // Baselib_ErrorState_Validate_PageRange already made sure that cast to size_t is safe.
        const size_t byteCount = (size_t)(pageSize * pageCount);

        // Tell kernel that it can free the memory in the given range.
        // Note that there is no guarantee on when freeing happens.
        // Subsequent accesses to the memory would repopulate it, but since we're also setting mprotect to PROT_NONE, this would result in a segfault.
        // MADV_FREE is a bit closer to what we want, but it is supported only in linux kernels >4.5
        //
        // In contrast to BSD's MADV_DONTNEED and POSIX_MADV_DONTNEED, on Linux MADV_DONTNEED means that the underlying physical pages may be reused for different allocations.
        if (pageState == Baselib_Memory_PageState_Reserved)
        {
            if (madvise(addressOfFirstPage, byteCount, MADV_DONTNEED))
                detail::RaiseError_madvise_mprotect(errorState, BASELIB_SOURCELOCATION);
        }
        else
        {
            if (madvise(addressOfFirstPage, byteCount, MADV_NORMAL))
                detail::RaiseError_madvise_mprotect(errorState, BASELIB_SOURCELOCATION);
        }

        if (mprotect(addressOfFirstPage, byteCount, protection) != 0)
            detail::RaiseError_madvise_mprotect(errorState, BASELIB_SOURCELOCATION);
    }
}
