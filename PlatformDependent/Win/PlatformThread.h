#pragma once

#include <windef.h>

#include "Runtime/Threads/ThreadId.h"
#include "Runtime/Utitlities/NonCopyable.h"
#include "Runtime/Utitlities/UnionTuple.h"

class PlatformThread : public NonCopyable
{
	friend class Thread;
	friend class ThreadHelper;
protected:
	static ThreadId GetCurrentThreadID();
};
