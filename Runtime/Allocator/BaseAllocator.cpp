#include "SourcePrefix.h"
#include "BaseAllocator.h"

#include "Runtime/Allocator/MemoryManager.h"

BaseAllocator::BaseAllocator(const char* name, bool isThreadSafe)
	: m_IsThreadSafe(isThreadSafe)
{
	memcpy(m_Name, name, (std::min)((int)kMaxNameLength, (int)strlen(name) + 1));
}

bool BaseAllocator::TryDeallocate(void* p)
{
	if (!Contains(p))
		return false;

	Deallocate(p);
	return true;
}
