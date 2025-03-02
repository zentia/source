#pragma once

#ifdef _MSC_VER
#define SOURCE_NOINLINE __declspec(noinline)
#define SOURCE_FORCEINLINE __forceinline
#endif

#define ANALYSIS_ASSUME(x) __analysis_assume(x)