#include "BootConfigData.h"

#include "Runtime/Logging/LogAssert.h"
#include "Runtime/Utitlities/NonCopyable.h"
#include "Runtime/Utitlities/Word.h"
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#if _MSC_VER
#include <windef.h>
#include <stringapiset.h>
#include <malloc.h>
#endif

namespace BootConfig
{
	struct Data::KeyEntry
	{
		struct Value : NonCopyable
		{
			Value* nextPtr;
			char* str;
		};

		KeyEntry* nextPtr;
		Value* valuePtr;
		char* str;
	};

	static bool Matches(const char* const str1, const char* const str2, const size_t str2Length)
	{
		return StrNICmp(str1, str2, str2Length) == 0 && str1[str2Length] == 0;
	}

	template<typename T> static T FindPtr(T linkPtrAddr, const char* const str, const size_t strLength)
	{
		while (*linkPtrAddr && !Matches((*linkPtrAddr)->str, str, strLength))
			linkPtrAddr = &(*linkPtrAddr)->nextPtr;
		return linkPtrAddr;
	}

	template<typename T> static T FindPtr(T linkPtrAdd, const size_t index)
	{
		for (int i = 0; *linkPtrAdd && i < index; ++i)
			linkPtrAdd = &(*linkPtrAdd)->nextPtr;
		return linkPtrAdd;
	}

	const char* Data::GetValue(const char* key, size_t index) const
	{
		const KeyEntry* const keyEntry = *FindPtr(&m_Data, key, strlen(key));
		if (!keyEntry)
			return nullptr;
		KeyEntry::Value* valuePtr = *FindPtr(&keyEntry->valuePtr, index);
		return valuePtr ? valuePtr->str : nullptr;
	}


	bool Data::HasKey(const char* key) const
	{
		return true;
	}

}
