#include "Include/Baselib.h"
#include "Source/WinApi/Baselib_RegisteredNetwork_WinApi_WithFallback.h"
#include "Source/WinApi/Baselib_Thread_WinApi.inl.h"
#include "Source/WinApi/Baselib_ThreadLocalStorage_TLS_WinApi.inl.h"

#if PLATFORM_FUTEX_NATIVE_SUPPORT
    #include "Source/WinApi/Baselib_SystemFutex_WinApi.inl.h"
#else
    #include "Source/Cpp11/Baselib_SystemFutex_Cpp11.inl.h"
#endif

namespace platform
{
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_Impl;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_IsEmulated;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Buffer_Register;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Buffer_Deregister;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Endpoint_Create;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Endpoint_GetNetworkAddress;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_Create;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_ScheduleRecv;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_ScheduleSend;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_ProcessSend;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_ProcessRecv;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedSend;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_WaitForCompletedRecv;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_DequeueRecv;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_DequeueSend;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_GetNetworkAddress;
    using WinApi_WithFallback::Baselib_RegisteredNetwork_Socket_UDP_Close;

    // Baselib_SystemFutex
#if PLATFORM_FUTEX_NATIVE_SUPPORT
    using WinApi::Baselib_SystemFutex_Wait;
    using WinApi::Baselib_SystemFutex_Notify;
#else
    using Cpp11Api::Baselib_SystemFutex_Wait;
    using Cpp11Api::Baselib_SystemFutex_Notify;
#endif

    // Baselib_Thread
    constexpr auto Baselib_Thread_Create = WinApi::Baselib_Thread_Create<WinApi::SetThreadName_ViaThreadDescriptionWithExceptionFallback>;
    using WinApi::Baselib_Thread_Join;
    using WinApi::Baselib_Thread_YieldExecution;
    using WinApi::Baselib_Thread_GetCurrentThreadId;
    BASELIB_INLINE_IMPL bool Baselib_Thread_SupportsThreads() { return true; }

    // Baselib_TLS
    using WinApi::TLS::Baselib_TLS_Alloc;
    using WinApi::TLS::Baselib_TLS_Free;
}

#include "Source/CProxy/Baselib_RegisteredNetwork_CProxy.inl.h"
#include "Source/CProxy/Baselib_SystemFutex_CProxy.inl.h"
#include "Source/CProxy/Baselib_Thread_CProxy.inl.h"
#include "Source/CProxy/Baselib_ThreadLocalStorage_CProxy.inl.h"
#include "Source/Baselib_Config_Common_WinApiBased.inl.h"
