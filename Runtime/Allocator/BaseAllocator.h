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
	virtual bool Contains(const void* p) const = 0;
protected:
	static const int kMaxNameLength = 64;
	char m_Name[kMaxNameLength];
private:
	bool m_IsThreadSafe;
};
