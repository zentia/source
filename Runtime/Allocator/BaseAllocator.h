#pragma once

#include "Runtime/Misc/AllocatorLabels.h"
#include "Runtime/Utitlities/NonCopyable.h"

enum class AllocatorMode
{
	Normal,
	TrackLeaks,
};

class BaseAllocator : public NonCopyable
{
public:
	BaseAllocator(const char* name, bool isThreadSafe = false);

	virtual void* Allocate(size_t size, int align) = 0;
	virtual void Deallocate(void* p) = 0;
	virtual bool TryDeallocate(void* p);
	virtual bool Contains(const void* p) const = 0;
	virtual size_t GetPtrSize(const void* ptr) const = 0;
protected:
	static const int kMaxNameLength = 64;
	char m_Name[kMaxNameLength];
private:
	bool m_IsThreadSafe;
};
