#include "SourcePrefix.h"
#include "BaseAllocator.h"

#include "Runtime/Allocator/MemoryManager.h"

BaseAllocator::BaseAllocator(const char* name, bool isThreadSafe)
	: m_IsThreadSafe(isThreadSafe)
{
	AssertMsg(strlen(name) < kMaxNameLength, "Allocator name is too long. Will truncate");
	memcpy(m_Name, name, (std::min)((int)kMaxNameLength, (int)strlen(name) + 1));
	m_AllocatorIdentifier = GetMemoryManager().RegisterAllocator(this);
	memset(m_FramePeakUsageRangeCount, 0, sizeof(m_FramePeakUsageRangeCount));
	m_FramePeakAllocatedBytes = 0;
}

bool BaseAllocator::TryDeallocate(void* p)
{
	if (!Contains(p))
		return false;

	Deallocate(p);
	return true;
}
