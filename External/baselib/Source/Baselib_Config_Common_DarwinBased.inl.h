#pragma once

#include "Source/Darwin/Baselib_Debug_DarwinApi.inl.h"
#include "Source/Darwin/Baselib_SystemSemaphore_DarwinApi.inl.h"
#include "Source/Darwin/Baselib_Thread_DarwinApi.inl.h"
#include "Source/Darwin/Baselib_Timer_DarwinApi.inl.h"

#include "Source/Posix/Baselib_DynamicLibrary_PosixApi.inl.h"
#include "Source/Posix/Baselib_ErrorState_PosixApi.inl.h"
#include "Source/Posix/Baselib_FileIO_PosixApi.inl.h"
#include "Source/Posix/Baselib_Socket_PosixApi.inl.h"
#include "Source/Posix/Baselib_Thread_PosixApi.inl.h"
#include "Source/Posix/Baselib_ThreadLocalStorage_PosixApi.inl.h"
#include "Source/Posix/Baselib_Timer_PosixApi.inl.h"

#include "Source/C99/Baselib_Process_C99Api.inl.h"

#include "Source/Cpp11/Baselib_Timer_Cpp11.inl.h"

#include "Source/Common/Baselib_RegisteredNetwork_Common.inl.h"
#include "Source/Common/Baselib_FileIO_Common.inl.h"

#if PLATFORM_FUTEX_NATIVE_SUPPORT
    #include "Source/Darwin/Baselib_SystemFutex_DarwinApi.inl.h"
#else
    #include "Source/Cpp11/Baselib_SystemFutex_Cpp11.inl.h"
#endif

namespace platform
{
    // Baselib_Debug
    using DarwinApi::Baselib_Debug_IsDebuggerAttached;

    // Baselib_ErrorState
    using PosixApi::Baselib_ErrorState_Explain;

    // Baselib_Process
    using C99Api::Baselib_Process_Abort;

    // Baselib_SystemSemaphore
    using DarwinApi::Baselib_SystemSemaphore_Create;
    using DarwinApi::Baselib_SystemSemaphore_CreateInplace;
    using DarwinApi::Baselib_SystemSemaphore_Acquire;
    using DarwinApi::Baselib_SystemSemaphore_TryAcquire;
    using DarwinApi::Baselib_SystemSemaphore_TryTimedAcquire;
    using DarwinApi::Baselib_SystemSemaphore_Release;
    using DarwinApi::Baselib_SystemSemaphore_Free;
    using DarwinApi::Baselib_SystemSemaphore_FreeInplace;

    // Baselib_RegisteredNetwork
    using Common::Baselib_RegisteredNetwork_Socket_UDP_Impl;
    using Common::Baselib_RegisteredNetwork_IsEmulated;
    using Common::Baselib_RegisteredNetwork_Buffer_Register;
    using Common::Baselib_RegisteredNetwork_Buffer_Deregister;
    using Common::Baselib_RegisteredNetwork_Endpoint_Create;
    using Common::Baselib_RegisteredNetwork_Endpoint_GetNetworkAddress;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_Create;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ScheduleRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ScheduleSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ProcessSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_ProcessRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_DequeueRecv;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_DequeueSend;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_GetNetworkAddress;
    using Common::Baselib_RegisteredNetwork_Socket_UDP_Close;

    // Baselib_TLS
    using PosixApi::Baselib_TLS_Alloc;
    using PosixApi::Baselib_TLS_Free;

    // Baselib_Timer
    using DarwinApi::Baselib_Timer_GetTicksToNanosecondsConversionRatio;
    using DarwinApi::Baselib_Timer_GetHighPrecisionTimerTicks;
    using Cpp11Api::Baselib_Timer_WaitForAtLeast;

    // Baselib_Socket
    using PosixApi::Baselib_Socket_Create;
    using PosixApi::Baselib_Socket_Bind;
    using PosixApi::Baselib_Socket_TCP_Connect;
    using PosixApi::Baselib_Socket_Poll;
    using PosixApi::Baselib_Socket_GetAddress;
    using PosixApi::Baselib_Socket_TCP_Listen;
    using PosixApi::Baselib_Socket_TCP_Accept;
    using PosixApi::Baselib_Socket_UDP_Send;
    using PosixApi::Baselib_Socket_TCP_Send;
    using PosixApi::Baselib_Socket_UDP_Recv;
    using PosixApi::Baselib_Socket_TCP_Recv;
    using PosixApi::Baselib_Socket_Close;

    // Baselib_SystemFutex
#if PLATFORM_FUTEX_NATIVE_SUPPORT
    using DarwinApi::Baselib_SystemFutex_Wait;
    using DarwinApi::Baselib_SystemFutex_Notify;
#else
    using Cpp11Api::Baselib_SystemFutex_Wait;
    using Cpp11Api::Baselib_SystemFutex_Notify;
#endif

    // Baselib_Thread
    constexpr auto Baselib_Thread_Create = PosixApi::Baselib_Thread_Create<DarwinApi::Thread_SetNameForCurrentThread>;
    using PosixApi::Baselib_Thread_Join;
    using PosixApi::Baselib_Thread_YieldExecution;
    using PosixApi::Baselib_Thread_GetCurrentThreadId;
    BASELIB_INLINE_IMPL bool Baselib_Thread_SupportsThreads() { return true; }

    // Baselib_FileIO
    using Common::Baselib_FileIO_EventQueue_Create;
    using Common::Baselib_FileIO_EventQueue_Free;
    using Common::Baselib_FileIO_EventQueue_Dequeue;
    using Common::Baselib_FileIO_EventQueue_Shutdown;
    using Common::Baselib_FileIO_AsyncOpen;
    using Common::Baselib_FileIO_AsyncRead;
    using Common::Baselib_FileIO_AsyncClose;
    using PosixApi::Baselib_FileIO_SyncOpen;
    using PosixApi::Baselib_FileIO_SyncFileFromNativeHandle;
    using PosixApi::Baselib_FileIO_SyncRead;
    using PosixApi::Baselib_FileIO_SyncWrite;
    using PosixApi::Baselib_FileIO_SyncFlush;
    using PosixApi::Baselib_FileIO_SyncSetFileSize;
    using PosixApi::Baselib_FileIO_SyncGetFileSize;
    using PosixApi::Baselib_FileIO_SyncClose;

    // Baselib_DynamicLibrary
    using PosixApi::Baselib_DynamicLibrary_OpenUtf8;
    using PosixApi::Baselib_DynamicLibrary_OpenUtf16;
    using PosixApi::Baselib_DynamicLibrary_OpenProgramHandle;
    using PosixApi::Baselib_DynamicLibrary_FromNativeHandle;
    using PosixApi::Baselib_DynamicLibrary_GetFunction;
    using PosixApi::Baselib_DynamicLibrary_Close;
}

#include "Source/CProxy/Baselib_Debug_CProxy.inl.h"
#include "Source/CProxy/Baselib_ErrorState_CProxy.inl.h"
#include "Source/CProxy/Baselib_SystemFutex_CProxy.inl.h"
#include "Source/CProxy/Baselib_Memory_CProxy.inl.h"
#include "Source/CProxy/Baselib_Process_CProxy.inl.h"
#include "Source/CProxy/Baselib_RegisteredNetwork_CProxy.inl.h"
#include "Source/CProxy/Baselib_Socket_CProxy.inl.h"
#include "Source/CProxy/Baselib_SystemSemaphore_CProxy.inl.h"
#include "Source/CProxy/Baselib_ThreadLocalStorage_CProxy.inl.h"
#include "Source/CProxy/Baselib_Thread_CProxy.inl.h"
#include "Source/CProxy/Baselib_Timer_CProxy.inl.h"
#include "Source/CProxy/Baselib_FileIO_CProxy.inl.h"
#include "Source/CProxy/Baselib_DynamicLibrary_CProxy.inl.h"
