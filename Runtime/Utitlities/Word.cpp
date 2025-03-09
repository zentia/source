#include "SourcePrefix.h"
#include "Configuration/SourceConfigure.h"
#include "Word.h"

#include <stdarg.h>

#include "Runtime/Logging/LogAssert.h"

int StrNICmp(const char* str1, const char* str2, size_t count)
{
	for (; count > 0; --count)
	{
		int c1 = (unsigned char)ToLower(*str1++);
		int c2 = (unsigned char)ToLower(*str2++);

		if (c1 == 0 || (c1 != c2))
			return c1 - c2;
	}

	return 0;
}

inline UInt64 StringToIntBase(core::string_ref s, bool& isNegative)
{
	isNegative = false;
	core::string_ref::iterator it = s.begin();
	while (it != s.end() && IsSpace(*it))
		it++;

	if (it == s.end())
		return 0U;

	switch (*it)
	{
	case '-': isNegative = true;
	case '+': it++;
	}

	UInt64 value = 0;

	for (; it != s.end(); ++it)
	{
		UInt64 a = *it - '0';

		if (a > 9)
			break;

		value = value * 10 + a;
	}

	return value;
}

template<class T>
T StringToUnsignedType(const core::string_ref s)
{
	bool negative = false;
	UInt64 value = StringToIntBase(s, negative);

	return negative ? 0U : (T)value;
}

core::string VFormat(const char* format, va_list ap)
{
	va_list zp;
	va_copy(zp, ap);
	char buffer[1024 * 10];
	vsnprintf(buffer, 1024 * 10, format, zp);
	va_end(zp);
	return core::string(buffer, kMemTempAlloc);
}

core::string Format(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	core::string formatted = VFormat(format, va);
	va_end(va);
	return formatted;
}