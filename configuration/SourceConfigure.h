#pragma once

#if ENABLE_STRIPPING_SOURCE_FILERNAMES
#define __FILE_STRIPPED__ ""
#else
#define __FILE_STRIPPED__ __FILE__
#endif

#define ENABLE_MEMORY_MANAGER 1

#ifndef ENABLE_PROFILER
#define ENABLE_PROFILER ((SOURCE_EDITOR) && 1)
#endif

#ifndef SOURCE_DYNAMIC_TLS
#define SOURCE_DYNAMIC_TLS 1
#endif
