#pragma once

#include "Runtime/Utitlities/NonCopyable.h"

#include <windef.h>
#include <handleapi.h>
#include <errhandlingapi.h>

namespace winutils
{
	EXTERN_C IMAGE_DOS_HEADER __ImageBase;

	inline HINSTANCE GetInstanceHandle()
	{
		return reinterpret_cast<HINSTANCE>(&__ImageBase);
	}
}
