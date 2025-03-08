#pragma once

#include "Source/ErrorStateBuilder.h"
#include "Source/ArgumentValidator.h"

#include <errhandlingapi.h>

// Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
BASELIB_CPP_INTERFACE
{
    struct WithGetLastError : ::detail::ErrorStateBuilder::InfoProvider<WithGetLastError>
    {
        DWORD value;

        WithGetLastError(DWORD setValue)
            : value(setValue)
        {
        }

        inline void Fill(Baselib_ErrorState& state) const
        {
            state.nativeErrorCodeType = Baselib_ErrorState_NativeErrorCodeType_GetLastError;
            state.nativeErrorCode = (uint64_t)value;
        }
    };
}

#define WithGetLastError(__expr) EnsureSystemErrorIsNotPassedDirectlyWithTwoNames(WithGetLastError, (__expr), (#__expr), "GetLastError()", "WSAGetLastError()")
