#include "windows_critical_section.h"



namespace source_platform::windows
{
	windows_critical_section::windows_critical_section()
	{
		InitializeCriticalSection(&m_critical_section_);
		// 设置临界区的自旋计数，自旋计数是指在一个线程尝试进入临界区时，如果该临界区已经被其他线程占用，线程会在进入休眠状态之前自旋的次数
		// 自旋是指在等待资源时不断循环检查资源是否可用，而不是进入休眠状态
		SetCriticalSectionSpinCount(&m_critical_section_, 4000);
	}

	windows_critical_section::~windows_critical_section()
	{
		DeleteCriticalSection(&m_critical_section_);
	}

	void windows_critical_section::lock()
	{
		EnterCriticalSection(&m_critical_section_);
	}

	bool windows_critical_section::try_lock()
	{
		return TryEnterCriticalSection(&m_critical_section_);
	}

	void windows_critical_section::unlock()
	{
		LeaveCriticalSection(&m_critical_section_);
	}

}
