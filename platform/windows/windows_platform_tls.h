#pragma once
#include <cstdint>

namespace source_platform::windows
{
	class windows_platform_tls
	{
	public:
		static uint32_t get_current_thread_id();
	};
}

typedef source_platform::windows::windows_platform_tls platform_tls;