#include "SourcePrefix.h"
#include "StackAllocator.h"

#include "Runtime/Allocator/MemoryManager.h"

StackAllocatorBase::StackAllocatorBase(void* blockStart, size_t blockSize, const char* name)
	: BaseAllocator(name, true)
	, m_Block(blockStart)
	, m_CurrentSize(blockSize)
{
	
}

StackAllocatorBase::StackAllocatorBase(void* blockStart, size_t initalBlockSize, size_t commitableBlockSize, const char* name, LowLevelVirtualAllocator* llAlloc)
	: BaseAllocator(name, true)
	, m_Block(blockStart)
	, m_CurrentSize(initalBlockSize)
{
	
}
