#pragma once
#include <string>
#include "Runtime/Utitlities/Annotations.h"

namespace StringTraits
{
	template<typename TChar, typename TStorage>
	inline const TChar* AsConstTChars(const core::basic_string<TChar, TStorage>& value)
	{
		return value.c_str();
	}

	template<typename TChar, typename TTraits, typename TAlloc>
	inline const TChar* AsConstTChars(const std::basic_string<TChar, TTraits, TAlloc>& value)
	{
		return value.c_str();
	}

	template<typename TChar>
	inline const TChar* AsConstTChars(const TChar* value)
	{
		return value;
	}
}
