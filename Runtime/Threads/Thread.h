#pragma once

#include "ThreadId.h"

#include "Runtime/Core/Containers/String.h"
#include "Runtime/Logging/LogAssert.h"

#include "Configuration/SourceConfigure.h"

#include "PlatformThread.h"

#include "Runtime/Utitlities/NonCopyable.h"

class Thread : public NonCopyable
{
public:
	static ThreadId mainThreadId;
};
