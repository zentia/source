#include <Include/Baselib.h>
#include <Include/Cpp/Atomic.h>

#include <cstdio>
#include <cstdarg>

BASELIB_C_INTERFACE
{
    static baselib::atomic<bool> g_Mute = false;

    BASELIB_API void detail_AssertLogMute(bool state)
    {
        g_Mute.store(state, baselib::memory_order_relaxed);
    }

    void detail_AssertLog(const char* format, ...)
    {
        if (g_Mute.load(baselib::memory_order_relaxed))
            return;
        va_list args;
        va_start(args, format);
        std::vfprintf(stderr, format, args);
        std::fflush(stderr);
        va_end(args);
    }
}
