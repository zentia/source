#pragma once

namespace memorysetup
{
	extern const char* kMainAllocatorBlockSizeString;
	extern const char* kThreadAllocatorBlockSizeString;
	const UInt64 kMainAllocatorBlockSize = 16 * 1024 * 1024;
	const UInt64 kThreadAllocatorBlockSize = 16 * 1024 * 1024;
}
