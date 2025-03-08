#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "Source/WinApi/ErrorStateBuilder_WinApi.inl.h"

#include "Include/C/Baselib_Memory.h"
#include "Include/C/Baselib_Process.h"
#include "Include/C/Baselib_StaticAssert.h"

#include "Source/Baselib_Memory_Utils.h"

#include <errhandlingapi.h>
#include <winerror.h>
#include <memoryapi.h>
#include <sysinfoapi.h>
#include <malloc.h>

namespace WinApi
{
namespace detail
{
    // Simplified bitwise translation of page staes in PageStateToNativeState require the relevant page states to be bit-wise exclusive and in 8-bit range
    // as well as ascending permissive order.
    BASELIB_STATIC_ASSERT(PAGE_NOACCESS == PAGE_READONLY >> 1 && PAGE_READONLY == PAGE_READWRITE >> 1 && PAGE_READWRITE == PAGE_WRITECOPY >> 1,
        "WinApi Memory Protection Constants incompatible: PAGE_NOACCESS, PAGE_READONLY, PAGE_READWRITE, PAGE_WRITECOPY flags expected to be bitwise exclusive and ascending permissive order");

    BASELIB_STATIC_ASSERT(PAGE_EXECUTE == PAGE_EXECUTE_READ >> 1 && PAGE_EXECUTE_READ == PAGE_EXECUTE_READWRITE >> 1 && PAGE_EXECUTE_READWRITE == PAGE_EXECUTE_WRITECOPY >> 1,
        "WinApi Memory Protection Constants incompatible: PAGE_EXECUTE, PAGE_EXECUTE_READ, PAGE_EXECUTE_READWRITE, PAGE_EXECUTE_WRITECOPY flags expected to be bitwise exclusive and ascending permissive order");

    BASELIB_STATIC_ASSERT(PAGE_WRITECOPY == PAGE_EXECUTE >> 1,
        "WinApi Memory Protection Constants incompatible: PAGE_EXECUTE flags expected to be bitwise exclusive and larger than PAGE flags.");

    static const uint32_t Native_PageStates_Executable = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;
    static const uint32_t Native_PageStates_NonExecutable = PAGE_NOACCESS | PAGE_READONLY | PAGE_READWRITE | PAGE_WRITECOPY;
    static const uint32_t Native_PageStates_Exclusive = Native_PageStates_Executable | Native_PageStates_NonExecutable;
    static const uint32_t Native_PageStates_ExecutableShift = 4; // Native_PageStates_Exclusive bit-range / 2

    static uint32_t PageStateToNativeState(Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        uint32_t nativeState;
        switch (pageState)
        {
            case Baselib_Memory_PageState_Reserved:
            case Baselib_Memory_PageState_NoAccess:
                nativeState = PAGE_NOACCESS;
                break;
            case Baselib_Memory_PageState_ReadOnly:
                nativeState = PAGE_READONLY;
                break;
            case Baselib_Memory_PageState_ReadWrite:
                nativeState = PAGE_READWRITE;
                break;
            case Baselib_Memory_PageState_ReadOnly_Executable:
                nativeState = PAGE_EXECUTE_READ;
                break;
            case Baselib_Memory_PageState_ReadWrite_Executable:
                nativeState = PAGE_EXECUTE_READWRITE;
                break;

            default:
                errorState |= RaiseError(Baselib_ErrorCode_UnsupportedPageState);
                return 0;
        }

        // Mask non-exclusive states for extended state.
        uint32_t extPageStateEx = extPageState & Native_PageStates_Exclusive;
        // If either state is executable, promote both to executable if needed
        uint32_t executableShift = ((nativeState & Native_PageStates_Executable) || (extPageStateEx & Native_PageStates_Executable)) ? Native_PageStates_ExecutableShift : 0;
        nativeState = (nativeState & Native_PageStates_Executable) ? nativeState : nativeState << executableShift;
        extPageStateEx = (extPageStateEx & Native_PageStates_Executable) ? extPageStateEx : extPageStateEx << executableShift;
        return ((nativeState > extPageStateEx) ? nativeState : extPageStateEx) | (extPageState & ~Native_PageStates_Exclusive);
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_RaiseVirtualMemoryError(Baselib_Memory_PageState pageState, Baselib_ErrorState* errorState, const DWORD lastError)
    {
        // In UWP applications we need a special capability ("codeGeneration") to support executable pages.
        // Checking this flag requires parsing the AppXManifest which would be slow and really out of place here.
        // So instead, we infer it from the error as well as we can.

        switch (lastError)
        {
            case ERROR_ACCESS_DENIED:        // Raised by VirtualProtect for Execute page when it is not allowed on UWP with no codeGeneration permission in manifest.
            case ERROR_DYNAMIC_CODE_BLOCKED: // Raised on XboxOne for Execute page when not enabled for device.
                errorState |= RaiseError(Baselib_ErrorCode_UnsupportedPageState) | WithGetLastError(lastError);
                return;

            case ERROR_INVALID_ADDRESS:
                errorState |= RaiseError(Baselib_ErrorCode_InvalidAddressRange) | WithGetLastError(lastError);
                return;

            case ERROR_NOT_ENOUGH_MEMORY:
            case ERROR_OUTOFMEMORY:
            case ERROR_COMMITMENT_LIMIT:
                errorState |= RaiseError(Baselib_ErrorCode_OutOfMemory) | WithGetLastError(lastError);
                return;

            case ERROR_INVALID_PARAMETER:
                // Raised by VirtualAlloc for executable pages on UWP if not supported.
                // Since we do a lot of parameter checking beforehand this is very likely the right error code.
                if (pageState == Baselib_Memory_PageState_ReadOnly_Executable || pageState == Baselib_Memory_PageState_ReadWrite_Executable)
                {
                    errorState |= RaiseError(Baselib_ErrorCode_UnsupportedPageState) | WithGetLastError(lastError);
                    return;
                }
        }
        BaselibAssert(false, "WinApi function failed with unexpected error: %d", lastError);
        errorState |= RaiseError(Baselib_ErrorCode_UnexpectedError) | WithGetLastError(lastError);
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_RaiseVirtualMemoryError(Baselib_ErrorState* errorState, const DWORD lastError)
    {
        Baselib_Memory_RaiseVirtualMemoryError(Baselib_Memory_PageState_ReadWrite, errorState, lastError);
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_GetPageSizeInfo(Baselib_Memory_PageSizeInfo* outPagesSizeInfo, const SYSTEM_INFO& systemInfo)
    {
        outPagesSizeInfo->pageSizes[0] = outPagesSizeInfo->defaultPageSize = static_cast<size_t>(systemInfo.dwPageSize);

        // Support for large pages is not guaranteed and requires the process have the 'SeLockMemoryPrivilege'
        // This privilege is disabled by default on Windows and querying whether we have it right now is not trivial.
        // For a good query example see: http://www.flounder.com/msdn_documentation_errors_and_omissions.htm#GetTokenInformation

/*
    // Windows distinguishes between between "pages" and "large pages".
    // Large pages may be larger than "large page minimum" but must always be a multiple of it, so we can just treat this as the only available large page.
#ifdef FILE_MAP_LARGE_PAGES
    SIZE_T largePageMinimum = GetLargePageMinimum();
    if (largePageMinimum != 0)
    {
        outPagesSizeInfo->pageSizes[1] = largePageMinimum;
        outPagesSizeInfo->pageSizesLen = 2;
    }
    else
#endif
*/
        outPagesSizeInfo->pageSizesLen = 1;
    }

    static inline bool Baselib_Memory_CheckWinApiReservationBounds(uint64_t totalReservedBytes)
    {
        // values higher than these trigger ERROR_INVALID_ADDRESS or ERROR_INVALID_PARAMETER for VirtualAlloc
        if (PLATFORM_ARCH_32 && totalReservedBytes >= 0xffff0000)
            return false;
        if (PLATFORM_ARCH_64 && totalReservedBytes >= 0x7fffffff0000)
            return false;
        return true;
    }
}

    BASELIB_INLINE_IMPL void Baselib_Memory_GetPageSizeInfo(Baselib_Memory_PageSizeInfo* outPagesSizeInfo)
    {
        SYSTEM_INFO systemInfo;
        GetNativeSystemInfo(&systemInfo);
        detail::Baselib_Memory_GetPageSizeInfo(outPagesSizeInfo, systemInfo);
    }

    BASELIB_INLINE_IMPL void* Baselib_Memory_AlignedAllocate(size_t size, size_t alignment)
    {
        void* allocatedMemory = _aligned_malloc(size, alignment);
        if (!allocatedMemory)
            Baselib_Process_Abort(errno == ENOMEM ? Baselib_ErrorCode_OutOfMemory : Baselib_ErrorCode_UnexpectedError);
        return allocatedMemory;
    }

    BASELIB_INLINE_IMPL void* Baselib_Memory_AlignedReallocate(void* ptr, size_t newSize, size_t alignment)
    {
        void* allocatedMemory = _aligned_realloc(ptr, newSize, alignment);
        if (OPTIMIZER_LIKELY(allocatedMemory))
            return allocatedMemory;
        if (newSize == 0)
            return Baselib_Memory_AlignedAllocate(newSize, alignment);

        Baselib_Process_Abort(errno == ENOMEM ? Baselib_ErrorCode_OutOfMemory : Baselib_ErrorCode_UnexpectedError);
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_AlignedFree(void* ptr)
    {
        _aligned_free(ptr);
    }

    BASELIB_INLINE_IMPL Baselib_Memory_PageAllocation Baselib_Memory_AllocatePagesEx(uint64_t pageSize, uint64_t pageCount, uint64_t alignmentInMultipleOfPageSize, Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        SYSTEM_INFO systemInfo;
        GetNativeSystemInfo(&systemInfo);
        detail::PageStateToNativeState(static_cast<Baselib_Memory_PageState>(pageState), extPageState, errorState);

        // Reservations (!) done by baselib are always aligned to the "allocation granularity".
        // Overflow checks for multiplying alignment & count with page size were already done in Baselib_ErrorState_Validate_PageRange.
        size_t totalReservedBytes;
        if (alignmentInMultipleOfPageSize * pageSize > systemInfo.dwAllocationGranularity)
            totalReservedBytes = Baselib_MemoryState_Utils_GetReservedByteCountWithAlignmentPadding(pageSize, pageCount, alignmentInMultipleOfPageSize, errorState);
        else
            totalReservedBytes = (size_t)(pageCount * pageSize);

        if (!detail::Baselib_Memory_CheckWinApiReservationBounds(totalReservedBytes))
            errorState |= RaiseError(Baselib_ErrorCode_OutOfMemory);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_Memory_PageAllocation_Invalid;

        // Notes on large pages:
        // * must be commited an reserved in a single operation.
        // * do not support PAGE_GUARD
        // Notes on specific address
        // * must be commited and reserved in a single operation unless entire region has been reserved

        // Large pages not supported currently.
        BaselibAssert(pageSize == systemInfo.dwPageSize);

        // Reserve with optional padding.
        // Note that we already checked that casting totalReservedBytes to size_t is safe via Baselib_Memory_CheckWinApiReservationBounds
        void* const reservedPages = VirtualAlloc(nullptr, totalReservedBytes, MEM_RESERVE, PAGE_NOACCESS);
        if (!reservedPages)
        {
            const auto lastError = GetLastError();
            detail::Baselib_Memory_RaiseVirtualMemoryError(errorState, lastError);
            return Baselib_Memory_PageAllocation_Invalid;
        }

        // Compute aligned pointer and set state.
        void* const alignedPointer = NextAlignedPointerIfNotAligned(reservedPages, alignmentInMultipleOfPageSize * pageSize);
        Baselib_Memory_SetPageStateEx(alignedPointer, pageSize, pageCount, pageState, extPageState, errorState);
        if (Baselib_ErrorState_ErrorRaised(errorState))
        {
            VirtualFree(reservedPages, 0, MEM_RELEASE);
            return Baselib_Memory_PageAllocation_Invalid;
        }

        // VirtualFree does not allow to free purely reserved memory. I.e. we are not able to free unused virtual address space before and after the aligned allocation block.
        // Documentation on MEM_RELEASE:
        // "If you specify this value, dwSize must be 0 (zero), and lpAddress must point to the base address returned by the VirtualAlloc function when the region is reserved. "
        // The alternative MEM_DECOMMIT does not apply since the part we want to free was not commited.
        // The only effect this has however, is that we waste virtual address space.

        return Baselib_Memory_PageAllocation { alignedPointer, pageSize, pageCount };
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_ReleasePages(Baselib_Memory_PageAllocation pageAllocation, Baselib_ErrorState* errorState)
    {
        // Check if this is a complete region
        _MEMORY_BASIC_INFORMATION memoryRegionInfo;
        if (VirtualQuery(pageAllocation.ptr, &memoryRegionInfo, sizeof(memoryRegionInfo)) != sizeof(memoryRegionInfo))
        {
            const auto lastError = GetLastError();
            detail::Baselib_Memory_RaiseVirtualMemoryError(errorState, lastError);
            return;
        }

        if (memoryRegionInfo.State == MEM_FREE)
        {
            errorState |= RaiseError(Baselib_ErrorCode_InvalidAddressRange);
            return;
        }

        // Free virtual address space of the entire region. This may include address padding we added due to an aligned
        if (!VirtualFree(memoryRegionInfo.AllocationBase, 0, MEM_RELEASE))
        {
            const auto lastError = GetLastError();
            detail::Baselib_Memory_RaiseVirtualMemoryError(errorState, lastError);
        }
    }

    BASELIB_INLINE_IMPL void Baselib_Memory_SetPageStateEx(void* addressOfFirstPage, uint64_t pageSize, uint64_t pageCount, Baselib_Memory_PageState pageState, uint32_t extPageState, Baselib_ErrorState* errorState)
    {
        uint32_t nativePageState = detail::PageStateToNativeState(pageState, extPageState, errorState);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;

        // We should be dealing with already allocated memory, but to avoid surprises we do winapi specific overflow checking here as well.
        const uint64_t totalSize = pageSize * pageCount;
        if (!detail::Baselib_Memory_CheckWinApiReservationBounds(totalSize))
            // Mark this as invalid range since apparently this range wasn't used before!
            errorState |= RaiseError(Baselib_ErrorCode_InvalidAddressRange);

        if (Baselib_ErrorState_ErrorRaised(errorState))
            return;


        if (pageState == Baselib_Memory_PageState_Reserved)
        {
            // There is no harm in decommitting already decommitted pages.
            if (!VirtualFree(addressOfFirstPage, (size_t)totalSize, MEM_DECOMMIT))
            {
                const auto lastError = GetLastError();
                detail::Baselib_Memory_RaiseVirtualMemoryError(errorState, lastError);
            }
        }
        else
        {
            // There is no harm in commiting already commited pages.
            void* const commitedPages = VirtualAlloc(addressOfFirstPage, (size_t)totalSize, MEM_COMMIT, (DWORD)nativePageState);
            if (!commitedPages)
            {
                const auto lastError = GetLastError();
                detail::Baselib_Memory_RaiseVirtualMemoryError(static_cast<Baselib_Memory_PageState>(pageState), errorState, lastError);
            }
            else
            {
                BaselibAssert(commitedPages == addressOfFirstPage);
            }
        }
    }
}
