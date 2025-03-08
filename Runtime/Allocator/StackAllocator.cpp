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

template <AllocatorMode allocMode>
void* StackAllocator<allocMode>::Allocate(size_t size, int align)
{
	align = AlignSize(align, kDefaultMemoryAlignment);

	const size_t alignmask = align - 1;

	return nullptr;
}

template class StackAllocator<AllocatorMode::Normal>;
template class StackAllocator<AllocatorMode::TrackLeaks>;