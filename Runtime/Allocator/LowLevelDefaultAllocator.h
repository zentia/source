#pragma once

class LowLevelVirtualAllocator
{
public:
	static const int kAllocatorIdentifierBits = 11;
	struct BlockInfo
	{
		BlockInfo() {}
		UInt32 allocatorIdentifier : kAllocatorIdentifierBits;
		UInt32 secondaryAllocatorIdentifier : kAllocatorIdentifierBits;
		UInt32 blockType : 2;
		UInt32 offset : 8;
	};

	static const size_t kReserveBlockGranularity = ((UInt64)1) << 28;
	static const size_t kHugeBlockSize = ((UInt64)1) << 40;
	static const size_t kMaxAddress = ((UInt64)1) << 52;
	static const size_t kHugeBlockCount = kMaxAddress / kHugeBlockSize;
	static const size_t kReserveBlockCount = kHugeBlockSize / kReserveBlockGranularity;

	virtual void* GetMemoryBlockFromPointer(const void* ptrInBlock) = 0;
	virtual BlockInfo GetBlockInfoFromPointer(const void* ptr) = 0;
};