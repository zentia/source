#pragma once

#include "Include/Cpp/CountdownTimer.h"
#include "Include/C/Baselib_Atomic.h"
#include <stdint.h>

using Baselib_Debug_IsDebuggerAttachedFuncPtr = bool(*)();

namespace detail
{
    static int8_t cachedState = 0;
    static baselib::CountdownTimer cacheTimeout(baselib::CountdownTimer::InitializeExpired());
}

// In cases if native implementation of IsDebuggerAttached is expensive,
// we would like to cache results of it every X seconds, in this case 1 second.
static inline bool Baselib_Debug_Cached_IsDebuggerAttached(Baselib_Debug_IsDebuggerAttachedFuncPtr func)
{
    BaselibAssert(func != nullptr);

    // Check if least than one second passed, then return cached state.
    if (!detail::cacheTimeout.TimeoutExpired())
        return detail::cachedState;

    // Need to check again if the debugger is attached and update cache
    int8_t curCache = detail::cachedState, debuggerAttached = func();
    if (Baselib_atomic_compare_exchange_strong_8_seq_cst_seq_cst_v(&detail::cachedState, &curCache, &debuggerAttached))
        detail::cacheTimeout = baselib::CountdownTimer::StartNew(std::chrono::seconds(1));

    return debuggerAttached;
}
