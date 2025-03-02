#pragma once

#include "baselib/Include/C/Baselib_Debug.h"

#ifndef DEBUG_BREAK
	#define DEBUG_BREAK do { if (Baselib_Debug_IsDebuggerAttached()) { Baselib_Debug_Break(); }} while (false)
#endif
