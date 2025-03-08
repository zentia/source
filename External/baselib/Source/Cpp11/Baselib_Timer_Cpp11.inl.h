#pragma once

#include "Source/Baselib_InlineImplementation.h"
#include "Include/C/Baselib_Timer.h"
#include "Include/Cpp/CountdownTimer.h"

#include <chrono>
#include <thread>

namespace Cpp11Api
{
    BASELIB_INLINE_IMPL Baselib_Timer_TickToNanosecondConversionRatio Baselib_Timer_GetTicksToNanosecondsConversionRatio()
    {
        using nano_ratio = std::ratio<std::chrono::steady_clock::period::num * Baselib_NanosecondsPerSecond, std::chrono::steady_clock::period::den>;
        return Baselib_Timer_TickToNanosecondConversionRatio { nano_ratio::num, nano_ratio::den };
    }

    BASELIB_INLINE_IMPL void Baselib_Timer_WaitForAtLeast(uint32_t timeInMilliseconds)
    {
        baselib::timeout_ms timeLeft(timeInMilliseconds);
        const baselib::CountdownTimer timer = baselib::CountdownTimer::StartNew(timeLeft);
        do
        {
            std::this_thread::sleep_for(timeLeft);
            timeLeft = timer.GetTimeLeftInMilliseconds();
        }
        while (timeLeft > baselib::timeout_ms::zero());
    }

    BASELIB_INLINE_IMPL Baselib_Timer_Ticks Baselib_Timer_GetHighPrecisionTimerTicks()
    {
        return std::chrono::steady_clock::now().time_since_epoch().count();
    }
}
