#pragma once

#include "Include/C/Baselib_Memory.h"
#include "Include/C/Baselib_Thread.h"
#include "Source/Baselib_Thread_Utils.h"
#include "Source/StringConversions.h"
#include "Source/WinApi/ErrorStateBuilder_WinApi.inl.h"
#include "Source/Baselib_Custom_Allocator.h"

#include <windows.h>

#include <cstring>
#include <string>

BASELIB_C_INTERFACE
{
    struct Baselib_Thread : Baselib_Thread_Common
    {
        Baselib_Thread(Baselib_Thread_EntryPointFunction threadEntryPoint, void* threadEntryPointArgument)
            : Baselib_Thread_Common(threadEntryPoint, threadEntryPointArgument), handle(nullptr) {}
        HANDLE handle;
    };
}

namespace WinApi
{
namespace detail
{
    static DWORD WINAPI ThreadEntryPoint(LPVOID baselibThread)
    {
        Baselib_Thread* thread = static_cast<Baselib_Thread*>(baselibThread);
        thread->entryPoint(thread->entryPointArgument);
        return 0;
    }
}
    // There was a bug in Visual Studio versions older than 2015 Update 2, where this function needed to be called from the created thread for the
    // debugger to catch the exception. We require that baselib is built with a newer VS version anyway, hence we call this function from the creator thread.
    //
    // Generally, we should use SetThreadDescription whenever possible, details see here:
    // https://docs.microsoft.com/en-us/visualstudio/debugger/how-to-set-a-thread-name-in-native-code?view=vs-2019
    static void SetThreadName_ViaException(Baselib_Thread* thread, const char* name)
    {
        const DWORD MS_VC_EXCEPTION = 0x406D1388;
    #pragma pack(push,8)
        typedef struct tagTHREADNAME_INFO
        {
            DWORD dwType; // Must be 0x1000.
            LPCSTR szName; // Pointer to name (in user addr space).
            DWORD dwThreadID; // Thread ID (-1=caller thread).
            DWORD dwFlags; // Reserved for future use, must be zero.
        } THREADNAME_INFO;
    #pragma pack(pop)
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name;
        info.dwThreadID = GetThreadId(thread->handle);
        info.dwFlags = 0;
        // needed to avoid C2712 if /EHsc enabled
        auto raiseException = [&info, MS_VC_EXCEPTION]()
            {
                __try
                {
                    RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
                }
                __except (EXCEPTION_EXECUTE_HANDLER)
                {
                }
            };
        raiseException();
    }

#if defined(NTDDI_WIN10_RS1) && NTDDI_VERSION >= NTDDI_WIN10_RS1

    static void SetThreadName_ViaThreadDescription(Baselib_Thread* thread, const char* name)
    {
        ::detail::string_container_128byte_utf16 stringContainer;
        SetThreadDescription(thread->handle, reinterpret_cast<const WCHAR*>(::detail::StrUtf8ToUtf16(name, stringContainer)));
    }

#endif

#if !BASELIB_PLATFORM_WINRT
    static void SetThreadName_ViaThreadDescriptionWithExceptionFallback(Baselib_Thread* thread, const char* name)
    {
        typedef HRESULT(WINAPI *SetThreadDescriptionFunc)(HANDLE, PCWSTR);
        static SetThreadDescriptionFunc setThreadDescription =
            reinterpret_cast<SetThreadDescriptionFunc>(GetProcAddress(GetModuleHandle(TEXT("kernel32.dll")), "SetThreadDescription"));

        if (setThreadDescription)
        {
            ::detail::string_container_128byte_utf16 stringContainer;
            setThreadDescription(thread->handle, reinterpret_cast<const WCHAR*>(::detail::StrUtf8ToUtf16(name, stringContainer)));
        }
        else
        {
            WinApi::SetThreadName_ViaException(thread, name);
        }
    }

#endif

    template<void(*SetThreadNameFunc)(Baselib_Thread* thread, const char* name)>
    BASELIB_INLINE_IMPL Baselib_Thread* Baselib_Thread_Create(Baselib_Thread_Config config, Baselib_ErrorState* errorState)
    {
        Baselib_Thread* thread = new(Baselib_Internal_Memory_Allocate(sizeof(Baselib_Thread)))
            Baselib_Thread(config.entryPoint, config.entryPointArgument);

        SECURITY_ATTRIBUTES* const defaultAttributes = nullptr;
        DWORD threadId;
        thread->handle = CreateThread(defaultAttributes, (size_t)config.stackSize, detail::ThreadEntryPoint, thread, CREATE_SUSPENDED, &threadId);

        if (thread->handle == nullptr)
        {
            const auto error = GetLastError();
            switch (error)
            {
                case ERROR_NOT_ENOUGH_MEMORY:
                case ERROR_MAX_THRDS_REACHED:
                    errorState |= RaiseError(Baselib_ErrorCode_OutOfSystemResources) | WithGetLastError(error);
                    break;
                case ERROR_INVALID_PARAMETER:
                default:
                    errorState |= RaiseError(Baselib_ErrorCode_UnexpectedError) | WithGetLastError(error);
                    BaselibAssert(false, "Could not create thread");
                    break;
            }

            Baselib_Internal_Memory_Free(thread);
            return nullptr;
        }

        thread->id = threadId;
        if (config.name)
            SetThreadNameFunc(thread, config.name);

        const auto resumeThreadResult = ResumeThread(thread->handle);
        if (resumeThreadResult == -1)
        {
            // "If the function fails, the return value is (DWORD) -1. To get extended error information, call GetLastError."
            const auto error = GetLastError();
            CloseHandle(thread->handle);
            Baselib_Internal_Memory_Free(thread);
            BaselibAssert(false, "Could not start thread");
            errorState |= RaiseError(Baselib_ErrorCode_UnexpectedError) | WithGetLastError(error);
            return nullptr;
        }

        return thread;
    }

    BASELIB_INLINE_IMPL void Baselib_Thread_Join(Baselib_Thread* thread, uint32_t timeoutInMilliseconds, Baselib_ErrorState* errorState)
    {
        HRESULT hr = WaitForSingleObject(thread->handle, timeoutInMilliseconds);
        switch (hr)
        {
            case WAIT_TIMEOUT:
                errorState |= RaiseError(Baselib_ErrorCode_Timeout);
                break;
            case WAIT_OBJECT_0:
                CloseHandle(thread->handle);
                Baselib_Internal_Memory_Free(thread);
                break;
            case WAIT_FAILED:
            default:
            {
                // WAIT_FAILED - "The function has failed. To get extended error information, call GetLastError."
                const auto error = GetLastError();
                errorState |= RaiseError(Baselib_ErrorCode_UnexpectedError) | WithGetLastError(error);
                BaselibAssert(false, "Could not join");
            }
        }
    }

    BASELIB_INLINE_IMPL void Baselib_Thread_YieldExecution()
    {
        SwitchToThread();
    }

    BASELIB_INLINE_IMPL Baselib_Thread_Id Baselib_Thread_GetCurrentThreadId()
    {
        return GetCurrentThreadId();
    }
}
