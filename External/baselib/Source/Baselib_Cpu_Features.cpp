#include "Include/Baselib.h"
#include "Include/C/Baselib_Cpu_Features.h"

#if COMPILER_MSVC
#include <intrin.h>
#endif

namespace detail
{
    BASELIB_CPP_INTERFACE
    {
        Detail_Baselib_Cpu_FeatureData_CpuId CpuId(const uint32_t level)
        {
            Detail_Baselib_Cpu_FeatureData_CpuId result = {};

            #if (COMPILER_CLANG || COMPILER_GCC) && defined(__x86_64__)
            asm volatile ("xchgq  %%rbx,%q1; cpuid; xchgq  %%rbx,%q1"
                : "=a"(result.regs[0]), "=r" (result.regs[1]), "=c"(result.regs[2]), "=d"(result.regs[3])
                : "0"(level), "1" (0), "2" (0));

            #elif (COMPILER_CLANG || COMPILER_GCC) && defined(__i386__)
            asm volatile ("cpuid"
                : "=a"(result.regs[0]), "=r" (result.regs[1]), "=c"(result.regs[2]), "=d"(result.regs[3])
                : "0"(level), "1" (0), "2" (0));

            #elif COMPILER_MSVC && (defined(_M_IX86) || defined(_M_X64))
            __cpuidex(result.regs, (int)level, 0);

            #endif

            return result;
        }
    }
}

BASELIB_C_INTERFACE
{
    BASELIB_API const Detail_Baselib_Cpu_FeatureData_CpuId Detail_Baselib_Cpu_FeatureData_CpuIdArray[Detail_Baselib_Cpu_FeatureData_CpuIdArrayLength] =
    {
        detail::CpuId(0x00000000),
        detail::CpuId(0x00000001),
        detail::CpuId(0x00000007),
        detail::CpuId(0x80000001)
    };
}
