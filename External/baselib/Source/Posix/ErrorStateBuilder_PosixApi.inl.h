#pragma once

#include "Source/ErrorStateBuilder.h"
#include "Source/ArgumentValidator.h"

#include <errno.h>

// Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
BASELIB_CPP_INTERFACE
{
    struct WithErrno : ::detail::ErrorStateBuilder::InfoProvider<WithErrno>
    {
        int value;

        WithErrno(int setValue)
            : value(setValue)
        {
        }

        inline void Fill(Baselib_ErrorState& state) const
        {
            state.nativeErrorCodeType = Baselib_ErrorState_NativeErrorCodeType_errno;
            state.nativeErrorCode = (uint64_t)value;
        }
    };
}

#define WithErrno(__expr) EnsureSystemErrorIsNotPassedDirectly(WithErrno, (__expr), (#__expr), "errno")
