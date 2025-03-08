#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "Include/C/Baselib_Memory.h"
#include "Source/Baselib_Memory_Utils.h"
#include "Source/Posix/ErrorStateBuilder_PosixApi.inl.h"

#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>

namespace PosixApi
{
namespace detail
{
    static void RaiseError_munmap(Baselib_ErrorState* errorState, const Baselib_SourceLocation& sourceLocation)
    {
        const int errnoErrorCode = errno;
        switch (errnoErrorCode)
        {
            case EINVAL:
                errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_InvalidAddressRange, sourceLocation) | WithErrno(errnoErrorCode);
                return;
        }
        BaselibAssert(false, "munmap call failed with unexpected error: %d", errnoErrorCode);
        errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnexpectedError, sourceLocation) | WithErrno(errnoErrorCode);
    }

    BASELIB_INLINE_IMPL void RaiseError_mmap(Baselib_ErrorState* errorState, const Baselib_SourceLocation& sourceLocation)
    {
        const int errnoErrorCode = errno;
        switch (errnoErrorCode)
        {
            // According to OpenGroup spec, EINVAL should not happen for out of memory.
            // However, the Linux man pages writes:
            // "We don't like addr, length, or offset (e.g., they are too large, or not aligned on a page boundary)."
            // This was actually observed on Apple Silicon (Mac Mini 2020, MacOS 11.2.3) when running x64 on Rosetta (when running native arm64, we get ENOMEM!)
            case EINVAL:
            case ENOMEM:
                errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_OutOfMemory, sourceLocation) | WithErrno(errnoErrorCode);
                return;
            case EACCES:
                errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnsupportedPageState, sourceLocation) | WithErrno(errnoErrorCode);
                return;
        }
        BaselibAssert(false, "mmap call failed with unexpected error: %d", errnoErrorCode);
        errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnexpectedError, sourceLocation) | WithErrno(errnoErrorCode);
    }

    template<bool SupportsExecutablePages = true>
    static uint32_t PageStateToNativeState(Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        switch (pageState)
        {
            case Baselib_Memory_PageState_Reserved:
            case Baselib_Memory_PageState_NoAccess:
                return PROT_NONE | extPageState;
            case Baselib_Memory_PageState_ReadOnly:
                return PROT_READ | extPageState;
            case Baselib_Memory_PageState_ReadWrite:
                return PROT_READ | PROT_WRITE | extPageState;

            case Baselib_Memory_PageState_ReadOnly_Executable:
                if (!SupportsExecutablePages)
                    errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnsupportedPageState, BASELIB_SOURCELOCATION);
                return PROT_EXEC | PROT_READ | extPageState;
            case Baselib_Memory_PageState_ReadWrite_Executable:
                if (!SupportsExecutablePages)
                    errorState |= RaiseErrorWithLocation(Baselib_ErrorCode_UnsupportedPageState, BASELIB_SOURCELOCATION);
                return PROT_EXEC | PROT_READ | PROT_WRITE | extPageState;
        }

        errorState |= RaiseError(Baselib_ErrorCode_UnsupportedPageState);
        return 0;
    }
}

    BASELIB_INLINE_IMPL void Baselib_Memory_GetPageSizeInfo(Baselib_Memory_PageSizeInfo* outPagesSizeInfo)
    {
        // There seems to be no pure POSIX way to retrieve large page sizes.
        //
        // Note that some platforms may vary their page size from device to device (even if pointer size is the same)
        // For example the iOS transition guide explicitely states "Never Hard-Code the Virtual Memory Page Size"
        // http://cdn.cocimg.com/cms/uploads/soft/130911/4196-130911095Z1.pdf
        outPagesSizeInfo->defaultPageSize = outPagesSizeInfo->pageSizes[0] = getpagesize();
        outPagesSizeInfo->pageSizesLen = 1;
    }

    template<bool SupportsExecutablePages = true, int additionalMmapFlags = 0>
    BASELIB_INLINE_IMPL Baselib_Memory_PageAllocation Baselib_Memory_AllocatePagesEx(uint64_t pageSize, uint64_t pageCount, uint64_t alignmentInMultipleOfPageSize,
        Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        const size_t totalReservedBytes = Baselib_MemoryState_Utils_GetReservedByteCountWithAlignmentPadding(pageSize, pageCount, alignmentInMultipleOfPageSize, errorState);
        int protection = static_cast<int>(detail::PageStateToNativeState<SupportsExecutablePages>(pageState, extPageState, errorState));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_Memory_PageAllocation_Invalid;

        const int flags = MAP_ANONYMOUS | MAP_PRIVATE | additionalMmapFlags;
        void* const mappedAddress = mmap(nullptr, totalReservedBytes, protection, flags, -1, 0);
        if (mappedAddress == MAP_FAILED)
        {
            detail::RaiseError_mmap(errorState, BASELIB_SOURCELOCATION);
            return Baselib_Memory_PageAllocation_Invalid;
        }

        uint8_t* const alignedPointer = (uint8_t*)NextAlignedPointerIfNotAligned(mappedAddress, alignmentInMultipleOfPageSize * pageSize);

        // Unmap memory before and after the aligned block.
        const size_t paddingSizeFront = alignedPointer - (uint8_t*)mappedAddress;
        BaselibAssert((paddingSizeFront % pageSize) == 0);
        BaselibAssert(paddingSizeFront + pageSize * pageCount <= totalReservedBytes);
        if (paddingSizeFront > 0)
        {
            if (munmap(mappedAddress, paddingSizeFront) != 0)
                detail::RaiseError_munmap(errorState, BASELIB_SOURCELOCATION);
        }
        const size_t paddingSizeBack = totalReservedBytes - paddingSizeFront - pageSize * pageCount;
        BaselibAssert((paddingSizeBack % pageSize) == 0);
        if (paddingSizeBack > 0)
        {
            if (munmap(alignedPointer + pageSize * pageCount, paddingSizeBack) != 0)
                detail::RaiseError_munmap(errorState, BASELIB_SOURCELOCATION);
        }

        BaselibAssert(IsAligned(alignedPointer, alignmentInMultipleOfPageSize * pageSize));
        BaselibAssert(mappedAddress <= alignedPointer);

        return Baselib_Memory_PageAllocation {alignedPointer, pageSize, pageCount};
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_ReleasePages(Baselib_Memory_PageAllocation pageAllocation, Baselib_ErrorState* errorState)
    {
        if (munmap(pageAllocation.ptr, pageAllocation.pageSize * pageAllocation.pageCount) != 0)
            detail::RaiseError_munmap(errorState, BASELIB_SOURCELOCATION);
    }
}
