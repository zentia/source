#pragma once
#include "ThreadId.h"

namespace CurrentThread
{
	ThreadId GetID();
	bool EqualsID(ThreadId thread);
	bool IsMainThread();
}
