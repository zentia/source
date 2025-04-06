#pragma once

#include <Windows.h>

namespace source_platform::windows
{
	// 临界区
	class windows_critical_section
	{
	public:
		windows_critical_section();
		~windows_critical_section();
		void lock();
		bool try_lock();
		void unlock();
	private:
		CRITICAL_SECTION m_critical_section_;
	};
}

typedef source_platform::windows::windows_critical_section critical_section;