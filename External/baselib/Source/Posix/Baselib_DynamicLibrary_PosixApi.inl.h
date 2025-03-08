#pragma once

#include "Include/Baselib.h"
#include "Include/Cpp/Baselib_DynamicLibrary.h"
#include "Include/C/Baselib_Process.h"
#include "Source/Baselib_InlineImplementation.h"
#include "Source/Baselib_Utilities.h"
#include "Source/StringConversions.h"
#include "Source/Posix/ErrorStateBuilder_PosixApi.inl.h"

#include <dlfcn.h>

// cannot static_assert to verify RTLD_DEFAULT != Baselib_DynamicLibrary_Handle_Invalid
// because reinterpret_cast is not allowed in static_assert and bionic defines RTLD_DEFAULT/RTLD_NEXT with reinterpret_cast

static inline bool IsValid(Baselib_DynamicLibrary_NativeHandleType nativeHandleType)
{
    return nativeHandleType == Baselib_DynamicLibrary_PosixDlopen;
}

namespace PosixApi
{
    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf8(
        const char* pathnameUtf8, // can be nullptr
        Baselib_ErrorState* errorState
    )
    {
        const auto dylib = dlopen(pathnameUtf8, RTLD_LAZY | RTLD_LOCAL);
        if (dylib != nullptr)
            return ::detail::AsBaselibHandle<Baselib_DynamicLibrary_Handle>(dylib);
        else
        {
            errorState |= RaiseError(Baselib_ErrorCode_FailedToOpenDynamicLibrary) | WithFormattedString("dlerror() = %s", dlerror());
            return Baselib_DynamicLibrary_Handle_Invalid;
        }
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf16(
        const baselib_char16_t* pathnameUtf16,
        Baselib_ErrorState* errorState
    )
    {
        ::detail::string_container_128byte_utf8 stringContainer;
        const auto pathnameUtf8 = ::detail::StrUtf16ToUtf8(pathnameUtf16, stringContainer);
        return PosixApi::Baselib_DynamicLibrary_OpenUtf8(pathnameUtf8, errorState);
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenProgramHandle(
        Baselib_ErrorState* errorState
    )
    {
        return PosixApi::Baselib_DynamicLibrary_OpenUtf8(nullptr, errorState);
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_FromNativeHandle(
        uint64_t handleRaw,
        uint32_t type,
        Baselib_ErrorState* errorState
    )
    {
        const auto nativeHandleType = static_cast<Baselib_DynamicLibrary_NativeHandleType>(type);
        errorState |= Validate(nativeHandleType);
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_DynamicLibrary_Handle_Invalid;

        switch (nativeHandleType)
        {
            case Baselib_DynamicLibrary_PosixDlopen:
            {
                // variable name must match public api argument name so we report correct invalid argument
                const auto handle = reinterpret_cast<void*>(handleRaw);
                // no reasonable checks can be made, any pointer value is potentially valid
                return ::detail::AsBaselibHandle<Baselib_DynamicLibrary_Handle>(handle);
            }
        }

        BaselibAssert(0); // not reachable
        return Baselib_DynamicLibrary_Handle_Invalid;
    }

    BASELIB_INLINE_IMPL void* Baselib_DynamicLibrary_GetFunction(
        Baselib_DynamicLibrary_Handle handle,
        const char* functionName,
        Baselib_ErrorState* errorState
    )
    {
        const auto dylib = ::detail::AsNativeType<void*>(handle);

        // must clear the error code
        dlerror();

        const auto func = dlsym(dylib, functionName);

        // func == nullptr is a valid value, so need to check for dlerror
        if (func == nullptr)
        {
            const char* error = dlerror();
            if (error != nullptr)
                errorState |= RaiseError(Baselib_ErrorCode_FunctionNotFound) | WithFormattedString("dlerror() = %s", error);
        }

        return func;
    }

    BASELIB_INLINE_IMPL void Baselib_DynamicLibrary_Close(
        Baselib_DynamicLibrary_Handle handle
    )
    {
        const auto dylib = ::detail::AsNativeType<void*>(handle);
        if (dlclose(dylib) != 0)
            Baselib_Process_Abort(Baselib_ErrorCode_UnexpectedError);
    }
}
