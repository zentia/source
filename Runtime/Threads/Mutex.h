#pragma once

#include "Runtime/Utitlities/NonCopyable.h"

class Mutex : NonCopyable
{
public:
	class AutoLock : NonCopyable
	{
		Mutex& m_Mutex;
	public:
		AutoLock(Mutex& mutex) : m_Mutex(mutex) {}
	};


};
