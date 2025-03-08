#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "../Posix/Baselib_Memory_PageAllocator_PosixApi.inl.h"

namespace DarwinApi
{
namespace detail
{
    // Note that the meaning for EINVAL/ENOMEM is different for madvise/mprotect compared with other related apis.
    // (also, on Linux based systems different errors are returned!)
    static void RaiseError_madvise_mprotect(Baselib_ErrorState* errorState, const Baselib_SourceLocation& sourceLocation)
    {
        const int errnoErrorCode = errno;
        switch (errnoErrorCode)
        {
            // EINVAL shows up on madvise either with unallocated regions or if the advice parameter is incorrect
            // (we controll the later directly so we assume this is not heppening here)
            case EINVAL:
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

    BASELIB_INLINE_IMPL Baselib_Memory_PageAllocation Baselib_Memory_AllocatePagesEx(uint64_t pageSize, uint64_t pageCount, uint64_t alignmentInMultipleOfPageSize, Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        if (pageState == Baselib_Memory_PageState_ReadOnly_Executable || pageState == Baselib_Memory_PageState_ReadWrite_Executable)
            return PosixApi::Baselib_Memory_AllocatePagesEx<true, MAP_JIT>(pageSize, pageCount, alignmentInMultipleOfPageSize, pageState, extPageState, errorState);
        else
            return PosixApi::Baselib_Memory_AllocatePagesEx(pageSize, pageCount, alignmentInMultipleOfPageSize, pageState, extPageState, errorState);
    }

    template<bool SupportsExecutablePages = true>
    BASELIB_INLINE_IMPL void Baselib_Memory_SetPageStateEx(void* addressOfFirstPage, uint64_t pageSize, uint64_t pageCount, Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        int protection = static_cast<int>(PosixApi::detail::PageStateToNativeState<SupportsExecutablePages>(static_cast<Baselib_Memory_PageState>(pageState), extPageState, errorState));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;

        // Baselib_ErrorState_Validate_PageRange already made sure that cast to size_t is safe.
        const size_t byteCount = (size_t)(pageSize * pageCount);

        if (pageState == Baselib_Memory_PageState_Reserved)
        {
            // We prefer MADV_FREE_REUSABLE over MADV_FREE where available (as of writing that is only on OSX).
            // It explicitly allows reuse by any other applications whereas MADV_FREE merely marks as freed.
            // Consequently, any tool showing the system's physical memory use, typically only shows memory marked with MADV_FREE_REUSABLE as available.
            if (madvise(addressOfFirstPage, byteCount, MADV_FREE_REUSABLE) != 0)
            {
                if (madvise(addressOfFirstPage, byteCount, MADV_FREE) != 0)
                    detail::RaiseError_madvise_mprotect(errorState, BASELIB_SOURCELOCATION);
            }
        }
        else
        {
            if (madvise(addressOfFirstPage, byteCount, MADV_NORMAL) != 0)
                detail::RaiseError_madvise_mprotect(errorState, BASELIB_SOURCELOCATION);
        }

        if (mprotect(addressOfFirstPage, byteCount, protection) != 0)
            detail::RaiseError_madvise_mprotect(errorState, BASELIB_SOURCELOCATION);
    }
}
