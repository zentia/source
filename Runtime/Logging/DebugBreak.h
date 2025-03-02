#pragma once

#include "baselib/Include/C/Baselib_Debug.h"

extern bool IsRunningNativeTests();

static inline bool IsDebuggerAttached()
{
	return Baselib_Debug_IsDebuggerAttached();
}

#ifndef DEBUG_BREAK
#define DEBUG_BREAK do {if (Baselib_Debug_IsDebuggerAttached()) {Baselib_Debug_Break();}}while(false)
#endif