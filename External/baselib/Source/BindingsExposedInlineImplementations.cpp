// Set BASELIB_BINDING_GENERATION, so all BASELIB_FORCEINLINE_API/BASELIB_API become regular apis.

//
// P/invoke only work if inline namespaces are disabled.
#if !defined(BASELIB_INLINE_NAMESPACE)

#define BASELIB_BINDING_GENERATION 1
#include "Include/Baselib.h"
#include "Include/C/Baselib_ThreadLocalStorage.h"
#include "Include/C/Baselib_SystemFutex.h"
#undef BASELIB_BINDING_GENERATION

#endif
