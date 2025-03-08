#pragma once

#include "Include/Cpp/Baselib_DynamicLibrary.h"
#include "Source/ArgumentValidator.h"

static inline bool IsValid(const Baselib_DynamicLibrary_Handle& handle)
{
    return handle != Baselib_DynamicLibrary_Handle_Invalid;
}

BASELIB_C_INTERFACE
{
    Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf8(
        const char* pathnameUtf8,
        Baselib_ErrorState* errorState
    )
    {
        errorState |= Validate(AsPointer(pathnameUtf8));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_DynamicLibrary_Handle_Invalid;

        return platform::Baselib_DynamicLibrary_OpenUtf8(pathnameUtf8, errorState);
    }

    Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenUtf16(
        const baselib_char16_t* pathnameUtf16,
        Baselib_ErrorState* errorState
    )
    {
        errorState |= Validate(AsPointer(pathnameUtf16));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_DynamicLibrary_Handle_Invalid;

        return platform::Baselib_DynamicLibrary_OpenUtf16(pathnameUtf16, errorState);
    }

    Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_OpenProgramHandle(
        Baselib_ErrorState* errorState
    )
    {
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return Baselib_DynamicLibrary_Handle_Invalid;

        return platform::Baselib_DynamicLibrary_OpenProgramHandle(errorState);
    }

    Baselib_DynamicLibrary_Handle Baselib_DynamicLibrary_FromNativeHandle(
        uint64_t handle,
        uint32_t type,
        Baselib_ErrorState* errorState
    )
    {
        return platform::Baselib_DynamicLibrary_FromNativeHandle(handle, type, errorState);
    }

    void* Baselib_DynamicLibrary_GetFunction(
        Baselib_DynamicLibrary_Handle handle,
        const char* functionName,
        Baselib_ErrorState* errorState
    )
    {
        errorState |= Validate(handle);
        errorState |= Validate(AsPointer(functionName));
        if (Baselib_ErrorState_ErrorRaised(errorState))
            return nullptr;
        return platform::Baselib_DynamicLibrary_GetFunction(handle, functionName, errorState);
    }

    void Baselib_DynamicLibrary_Close(
        Baselib_DynamicLibrary_Handle handle
    )
    {
        if (!IsValid(handle))
            return;
        return platform::Baselib_DynamicLibrary_Close(handle);
    }
}
