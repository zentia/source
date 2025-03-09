#pragma once

#include "Runtime/Utitlities/NonCopyable.h"

namespace BootConfig
{
	class Data : NonCopyable
	{
	public:
		const char* GetValue(const char* key, size_t index = 0) const;
		bool HasKey(const char* key) const;
		struct KeyEntry;
	private:
		KeyEntry* m_Data;
	};
}
