#pragma once

#include "Runtime/Utitlities/NonCopyable.h"

#if PLATFORM_HAS_CUSTOM_MUTEX
#include "PlatformMutex.h"
#else
#include "baselib/Include/Cpp/ReentrantLock.h"
using PlatformMutex = baselib::ReentrantLock;
#endif

class Mutex : NonCopyable
{
public:
	class AutoLock : NonCopyable
	{
		Mutex& m_Mutex;
	public:
		AutoLock(Mutex& mutex) : m_Mutex(mutex) {}
	};

	FORCE_INLINE void Lock() { m_Mutex.Acquire(200); }
	FORCE_INLINE void Unlock() { m_Mutex.Release(); }

	// Returns true if locking succeeded
	FORCE_INLINE bool TryLock() { return m_Mutex.TryAcquire(); }

	FORCE_INLINE void BlockUntilUnlocked()
	{
		Lock();
		Unlock();
	}
private:
	PlatformMutex m_Mutex;
};
