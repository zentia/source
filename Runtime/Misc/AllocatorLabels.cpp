#include "SourcePrefix.h"
#include "AllocatorLabels.h"

MemLabelId SetCurrentMemoryOwner(MemLabelRef label)
{
	return label;
}

const AllocationRootWithSalt AllocationRootWithSalt::kNoRoot = { 0, AllocationRootWithSalt::kNotInList };