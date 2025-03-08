#pragma once

#include "Include/Baselib.h"
#include "Include/C/Baselib_DynamicLibrary.h"
#include "Include/C/Baselib_Process.h"
#include "Source/Baselib_InlineImplementation.h"
#include "Source/Baselib_Utilities.h"
#include "Source/StringConversions.h"
#include "Source/WinApi/ErrorStateBuilder_WinApi.inl.h"

#include <windows.h>

static inline bool IsValid(Baselib_DynamicLibrary_NativeHandleType nativeHandleType)
{
    return nativeHandleType == Baselib_DynamicLibrary_WinApiHMODULE;
}

namespace WinApi
{
    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf16(
        const baselib_char16_t* pathnameUtf16,
        Baselib_ErrorState* errorState
    )
    {
        #if BASELIB_PLATFORM_WINRT
        const auto module = LoadPackagedLibrary(reinterpret_cast<const WCHAR*>(pathnameUtf16), 0);
        #else
        const auto module = LoadLibraryW(reinterpret_cast<const WCHAR*>(pathnameUtf16));
        #endif

        if (module != NULL)
            return ::detail::AsBaselibHandle<Baselib_DynamicLibrary_Handle>(module);
        else
        {
            const auto lastError = GetLastError();
            errorState |= RaiseError(Baselib_ErrorCode_FailedToOpenDynamicLibrary) | WithGetLastError(lastError);
            return Baselib_DynamicLibrary_Handle_Invalid;
        }
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf8(
        const char* pathnameUtf8,
        Baselib_ErrorState* errorState
    )
    {
        ::detail::string_container_128byte_utf16 stringContainer;
        auto pathnameUtf16 = ::detail::StrUtf8ToUtf16(pathnameUtf8, stringContainer);
        return WinApi::Baselib_DynamicLibrary_OpenUtf16(pathnameUtf16, errorState);
    }

    BASELIB_INLINE_IMPL Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenProgramHandle(
        Baselib_ErrorState* errorState
    )
    {
        errorState |= RaiseError(Baselib_ErrorCode_NotSupported);
        return Baselib_DynamicLibrary_Handle_Invalid;
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
            case Baselib_DynamicLibrary_WinApiHMODULE:
            {
                // variable name must match public api argument name so we report correct invalid argument
                const auto handle = reinterpret_cast<HMODULE>(handleRaw);
                errorState |= Validate(AsPointer(handle));
                if (Baselib_ErrorState_ErrorRaised(errorState))
                    return Baselib_DynamicLibrary_Handle_Invalid;

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
        const auto module = ::detail::AsNativeType<HMODULE>(handle);
        const auto func = GetProcAddress(module, functionName);
        if (func == NULL)
        {
            const auto lastError = GetLastError();
            if (lastError == ERROR_PROC_NOT_FOUND)
                errorState |= RaiseError(Baselib_ErrorCode_FunctionNotFound) | WithGetLastError(lastError);
            else
                errorState |= RaiseError(Baselib_ErrorCode_UnexpectedError) | WithGetLastError(lastError);
        }
        return func;
    }

    BASELIB_INLINE_IMPL void Baselib_DynamicLibrary_Close(
        Baselib_DynamicLibrary_Handle handle
    )
    {
        // FYI: In some cases libil2cpp uses Baselib_DynamicLibrary_OpenProgramHandle without calling Baselib_DynamicLibrary_Close.
        // So if in the future this implementation will need to allocate any resources, then users of baselib need to be fixed first.
        const auto module = ::detail::AsNativeType<HMODULE>(handle);
        if (FreeLibrary(module) == 0)
            Baselib_Process_Abort(Baselib_ErrorCode_UnexpectedError);
    }
}
