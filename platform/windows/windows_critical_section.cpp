#include "windows_critical_section.h"



namespace source_platform::windows
{
	windows_critical_section::windows_critical_section()
	{
		InitializeCriticalSection(&m_critical_section_);
		// �����ٽ�������������������������ָ��һ���̳߳��Խ����ٽ���ʱ��������ٽ����Ѿ��������߳�ռ�ã��̻߳��ڽ�������״̬֮ǰ�����Ĵ���
		// ������ָ�ڵȴ���Դʱ����ѭ�������Դ�Ƿ���ã������ǽ�������״̬
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
