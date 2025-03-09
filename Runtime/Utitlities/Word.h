#pragma once

#include "Runtime/Core/Containers/String.h"
#include "Runtime/Core/Containers/StringRef.h"

template<typename TString>
bool StrIEquals(core::string_ref str1, const TString& str2)
{
	return str1.compare(str2, kComparisonIgnoreCase) == 0;
}
inline char ToLower(char c) { return (c >= 'A' && c <= 'Z') ? (c + 'a' - 'A') : c; }

int StrNICmp(const char* str1, const char* str2, size_t count);

inline bool IsSpace(char c) { return c == '\t' || c == '\n' || c == '\f' || c == '\r' || c == ' '; }