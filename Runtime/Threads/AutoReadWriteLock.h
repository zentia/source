#pragma once

#include "Runtime/Utitlities/NonCopyable.h"

template <class TLock>
class AutoReadLockT : NonCopyable
{
public:
	AutoReadLockT(TLock& rwslock) : m_rwsLock(rwslock) { m_rwsLock.ReadLock(); }
	~AutoReadLockT() { m_rwsLock.ReadUnlock(); }
private:
	TLock& m_rwsLock;
};

template <class TLock>
class AutoWriteLockT : NonCopyable
{
public:
	AutoWriteLockT(TLock& rwslock) : m_rwsLock(rwslock) { m_rwsLock.WriteLock(); }
	~AutoWriteLockT() { m_rwsLock.WriteUnlock(); }
private:
	TLock& m_rwsLock;
};
