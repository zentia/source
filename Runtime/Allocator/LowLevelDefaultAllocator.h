#pragma once

class LowLevelVirtualAllocator
{
public:
	LowLevelVirtualAllocator();

	static const int kAllocatorIdentifierBits = 11;
	struct BlockInfo
	{
		BlockInfo() : allocatorIdentifier(0), secondaryAllocatorIdentifier(0), blockType(0), offset(0) {}
		UInt32 allocatorIdentifier : kAllocatorIdentifierBits;
		UInt32 secondaryAllocatorIdentifier : kAllocatorIdentifierBits;
		UInt32 blockType : 2;
		UInt32 offset : 8;
	};
	virtual void* ReserveMemoryBlock(size_t size, BlockInfo info) = 0;
	virtual void ReleaseMemoryBlock(void* ptr, size_t size) = 0;
	virtual size_t CommitMemory(void* ptr, size_t size) = 0;
	virtual size_t DecommitMemory(void* ptr, size_t size) = 0;

	static const size_t kReserveBlockGranularity = ((UInt64)1) << 28;
	static const size_t kHugeBlockSize = ((UInt64)1) << 40;
	static const size_t kMaxAddress = ((UInt64)1) << 52;
	static const size_t kHugeBlockCount = kMaxAddress / kHugeBlockSize;
	static const size_t kReserveBlockCount = kHugeBlockSize / kReserveBlockGranularity;

	virtual void* GetMemoryBlockFromPointer(const void* ptrInBlock) = 0;
	virtual BlockInfo GetBlockInfoFromPointer(const void* ptr) = 0;
};