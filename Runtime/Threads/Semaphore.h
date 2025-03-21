#pragma once

#include "baselib/Include/Cpp/Semaphore.h"
#include "Runtime/Utitlities/NonCopyable.h"

class Semaphore : public NonCopyable
{
public:
	bool WaitForSingle(int timeoutMs = -1)
	{
		return WaitForSignalNoProfile(timeoutMs);
	}

	bool WaitForSignalNoProfile(int timeoutMs = -1)
	{
		if (OPTIMIZER_LIKELY(timeoutMs == -1))
		{
			m_Semaphore.Acquire();
			return true;
		}
		return m_Semaphore.TryTimedAcquire(baselib::timeout_ms((uint32_t)timeoutMs));
	}

	void Signal(int count = 1) { m_Semaphore.Release(count); }
private:
	baselib::Semaphore m_Semaphore;
};
