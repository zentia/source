#include "SourcePrefix.h"

#include "baselib/Include/Cpp/Atomic.h"
#include "Thread.h"
#include "ThreadHelper.h"
#include "CurrentThread.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "Runtime/Utitlities/Utility.h"
#include "Runtime/Utitlities/Word.h"

#include "Runtime/Logging/LogAssert.h"
#include "Runtime/Allocator/MemoryMacros.h"

ThreadId Thread::mainThreadId = CurrentThread::GetID();