#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "Include/Cpp/Atomic.h"
#include "Include/C/Baselib_Timer.h"

#import <time.h>

namespace detail
{
    // This construct is here to ensure that Baselib_Timer_GetTimeSinceStartupInSeconds is called during static initialization
    static struct Darwin_Timer_StartupTimeInitializer
    {
        Darwin_Timer_StartupTimeInitializer()
        {
            Baselib_Timer_GetTimeSinceStartupInSeconds();
        }
    } darwin_Timer_StartupTimeInitializer;
}

namespace DarwinApi
{
    BASELIB_INLINE_IMPL Baselib_Timer_TickToNanosecondConversionRatio Baselib_Timer_GetTicksToNanosecondsConversionRatio()
    {
        return Baselib_Timer_TickToNanosecondConversionRatio{1, 1};
    }

    BASELIB_INLINE_IMPL Baselib_Timer_Ticks Baselib_Timer_GetHighPrecisionTimerTicks()
    {
        return clock_gettime_nsec_np(CLOCK_UPTIME_RAW);
    }

    BASELIB_INLINE_IMPL double Baselib_Timer_GetTimeSinceStartupInSeconds()
    {
        UNUSED(::detail::darwin_Timer_StartupTimeInitializer);
        const uint64_t nsSinceBoot = clock_gettime_nsec_np(CLOCK_MONOTONIC_RAW);

        static const uint64_t nsSinceBootAtStartup = nsSinceBoot;
        return (nsSinceBoot - nsSinceBootAtStartup) * 1e-9;
    }
}
