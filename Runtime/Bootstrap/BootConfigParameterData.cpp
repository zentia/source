#include "SourcePrefix.h"
#include "BootConfigParameterData.h"
#include "Runtime/Utitlities/Word.h"

namespace BootConfig
{
	template <>
	const char* ParameterParser<const char*>::Parse(const char* value, const char* defaultValue) const
	{
		return value;
	}

	template<typename T>
	static T ParseWithScanf(const char* format, const char* value, T defaultValue)
	{
		T result;
		if (value && sscanf(value, format, &result) == 1)
			return result;
		return defaultValue;
	}

	template <>
	unsigned long long ParameterParser<unsigned long long>::Parse(const char* value, unsigned long long defaultValue) const
	{
		return ParseWithScanf("%llu", value, defaultValue);
	}


}
