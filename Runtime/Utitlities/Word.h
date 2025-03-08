#pragma once

#include "Runtime/Core/Containers/String.h"
#include "Runtime/Core/Containers/StringRef.h"

template<typename TString>
bool StrIEquals(core::string_ref str1, const TString& str2)
{
	return str1.compare(str2, kComparisonIgnoreCase) == 0;
}
