#pragma once

#include "Include/Cpp/Stopwatch.h"

namespace Common
{
namespace detail
{
    // Put internal classes under inline namespace when active to avoid potentially duplicated symbols.
    BASELIB_CPP_INTERFACE
    {
        // This construct is here to ensure that GetTimeSinceStartup_Common is called during static initialization
        static struct CommonStartupTimeInitializer
        {
            CommonStartupTimeInitializer()
            {
                Baselib_Timer_GetTimeSinceStartupInSeconds();
            }
        } commonStartupTimeInitializer;
    }
}

    static double Baselib_Timer_GetTimeSinceStartupInSeconds()
    {
        UNUSED(detail::commonStartupTimeInitializer);
        // The reason for having this as a scoped static and not a file static is to avoid the ordering
        // problems of file static initialization.
        static const baselib::Stopwatch s_Stopwatch = baselib::Stopwatch::StartNew();
        return std::chrono::duration<double>(s_Stopwatch.GetElapsedTime()).count();
    }
}
