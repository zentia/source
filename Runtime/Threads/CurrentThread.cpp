#include "SourcePrefix.h"

#include "CurrentThread.h"
#include "ThreadHelper.h"

namespace CurrentThread
{
	ThreadId GetID()
	{
		return ThreadHelper::GetCurrentThreadId();
	}

	bool EqualsID(ThreadId thread)
	{
		return GetID() == thread;
	}

	bool IsMainThread()
	{
		return EqualsID(Thread::mainThreadId);
	}
}
