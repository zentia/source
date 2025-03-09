#pragma once

#include "Baselib_WakeupFallbackStrategy.h"

#ifdef __cplusplus
BASELIB_C_INTERFACE
{
#endif

BASELIB_INLINE_API bool Baselib_SystemFutex_NativeSupport(void) { return PLATFORM_FUTEX_NATIVE_SUPPORT == 1; }

BASELIB_API void Baselib_SystemFutex_Wait(int32_t* address, int32_t expected, uint32_t timeoutInMilliseconds);

BASELIB_API void Baselib_SystemFutex_Notify(int32_t* address, uint32_t count, Baselib_WakeupFallbackStrategy wakeupFallbackStrategy);

#ifdef __cplusplus
} // BASELIB_C_INTERFACE
#endif
