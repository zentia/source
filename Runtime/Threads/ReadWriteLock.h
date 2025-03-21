#pragma once

#include "AutoReadWriteLock.h"
#include "ExtenedAtomicOps.h"
#include "Semaphore.h"

#include "Runtime/Utitlities/NonCopyable.h"

class ReadWriteLock : NonCopyable
{
public:
	typedef AutoReadLockT<ReadWriteLock> AutoReadLock;
	typedef AutoWriteLockT<ReadWriteLock> AutoWriteLock;

	ReadWriteLock();

	void ReadLock();
	void ReadUnlock();

	void WriteLock();
	void WriteUnlock();
private:
	enum
	{
		kAtomicWordBits = sizeof(atomic_word) * 8,

		// As we have readers-preferred lock it's better to use more bits for readers counters.
		// And readers and waitingReaders should have the same amount of bits used.
		kReadersBits = (kAtomicWordBits - kAtomicWordBits / 3) / 2,
		kWritersBits = kAtomicWordBits - (kReadersBits * 2),

		kMaxReadersCount = (1 << kReadersBits) - 1,
		kMaxWritersCount = (1 << kWritersBits) - 1,
	};
	static_assert(kWritersBits + kReadersBits * 2 == kAtomicWordBits, "ReadWriteLock readers and writers counters should fit atomic_word");

	union ReadWriteStatus
	{
		struct
		{
			atomic_word readers : kReadersBits;
			atomic_word waitingReaders : kReadersBits;
			atomic_word writers : kWritersBits;
		};
		atomic_word data;
	};

	alignas(8) volatile atomic_word m_ReadersWritersStatus;
	Semaphore m_ReadSemaphore;
	Semaphore m_WriteSemaphore;
};

inline ReadWriteLock::ReadWriteLock() : m_ReadersWritersStatus(0)
{

}

inline void ReadWriteLock::ReadLock()
{
	ReadWriteStatus status, oldStatus;
	oldStatus.data = atomic_load_explicit(&m_ReadersWritersStatus, ::memory_order_relaxed);
	do
	{
		status = oldStatus;
		if (status.writers > 0)
		{
			AssertMsg(status.waitingReaders < kMaxReadersCount, "ReadWriteLock::waitingReaders overflow!");
			status.waitingReaders++;
		}
		else
		{
			AssertMsg(status.readers < kMaxReadersCount, "ReadWriteLock::readers overflow!");
			status.readers++;
		}
	} while (!atomic_compare_exchange_weak_explicit(&m_ReadersWritersStatus, &oldStatus.data, status.data, ::memory_order_acquire, ::memory_order_relaxed));

	if (oldStatus.writers > 0)
	{
		m_ReadSemaphore.WaitForSingle();
	}
}

inline void ReadWriteLock::ReadUnlock()
{
	ReadWriteStatus status, oldStatus;
	oldStatus.data = atomic_load_explicit(&m_ReadersWritersStatus, ::memory_order_relaxed);
	do
	{
		status = oldStatus;
		AssertMsg(status.readers > 0, "ReadWriteLoc::readers underflow!");
		status.readers--;
	}
	while (!atomic_compare_exchange_weak_explicit(&m_ReadersWritersStatus, &oldStatus.data, status.data, ::memory_order_acquire, ::memory_order_relaxed));
	if (oldStatus.readers == 1 && oldStatus.writers > 0)
		m_WriteSemaphore.Signal();
}

inline void ReadWriteLock::WriteLock()
{
	ReadWriteStatus status, oldStatus;
	oldStatus.data = atomic_load_explicit(&m_ReadersWritersStatus, ::memory_order_relaxed);
	do
	{
		status = oldStatus;
		AssertMsg(status.writers < kMaxWritersCount, "ReadWriteLock::writers overflow!");
		status.writers++;
	}
	while (!atomic_compare_exchange_weak_explicit(&m_ReadersWritersStatus, &oldStatus.data, status.data, ::memory_order_acquire, ::memory_order_relaxed));
	if (oldStatus.readers > 0 || oldStatus.writers > 0)
	{
		m_WriteSemaphore.WaitForSingle();
	}
}


inline void ReadWriteLock::WriteUnlock()
{
	ReadWriteStatus status, oldStatus;
	oldStatus.data = atomic_load_explicit(&m_ReadersWritersStatus, ::memory_order_relaxed);
	do
	{
		status = oldStatus;
		AssertMsg(oldStatus.writers > 0, "ReadWriteLock::writers underflow!");
		status.writers--;
		if (status.waitingReaders > 0)
		{
			status.readers = status.waitingReaders;
			status.waitingReaders = 0;
		}
	}
	while (!atomic_compare_exchange_weak_explicit(&m_ReadersWritersStatus, &oldStatus.data, status.data, ::memory_order_relaxed, ::memory_order_relaxed));
	if (status.readers > 0)
	{
		int readers = status.readers;
		while (readers-- > 0)
		{
			m_ReadSemaphore.Signal();
		}
	}
	else if (status.writers > 0)
	{
		m_WriteSemaphore.Signal();
	}
}
