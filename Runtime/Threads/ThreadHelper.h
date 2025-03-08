#pragma once

#include "Thread.h"
#include "CurrentThread.h"

class ThreadHelper
{
	friend class Thread;
	friend class PlatformThread;
	friend ThreadId CurrentThread::GetID();
protected:
	static ThreadId GetCurrentThreadId()
	{
		return PlatformThread::GetCurrentThreadID();
	}
};
