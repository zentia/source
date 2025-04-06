#include "windows_platform_tls.h"
#include <Windows.h>

namespace source_platform::windows
{
	uint32_t windows_platform_tls::get_current_thread_id()
	{
		return GetCurrentThreadId();
	}

}
