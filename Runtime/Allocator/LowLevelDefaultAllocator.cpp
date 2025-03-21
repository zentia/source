#include "SourcePrefix.h"
#include "LowLevelDefaultAllocator.h"
#include "Runtime/Allocator/MemoryManager.h"
#include "baselib/Include/C/Baselib_Memory.h"

static_assert(sizeof(LowLevelVirtualAllocator::BlockInfo) == sizeof(UINT32), "BlockInfo should have the size of a UInt32. Check that bitmasks add up 32");

LowLevelVirtualAllocator::LowLevelVirtualAllocator()
{
	Baselib_Memory_PageSizeInfo info;
	Baselib_Memory_GetPageSizeInfo(&info);
	m_DefaultPageSize = info.defaultPageSize;
}
