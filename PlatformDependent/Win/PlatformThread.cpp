#include "SourcePrefix.h"
#include "PlatformThread.h"

#include "Runtime/Threads/CurrentThread.h"

#include <windef.h>
#include <WinBase.h>

ThreadId PlatformThread::GetCurrentThreadID()
{
	return bit_cast_to_maybe_larger<ThreadId>(GetCurrentThreadId());
}
