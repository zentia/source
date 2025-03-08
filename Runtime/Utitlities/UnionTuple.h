#pragma once

#include "Runtime/Utitlities/StaticAssert.h"
#include <cstring>

template<class Target, class Source>
SOURCE_FORCEINLINE Target bit_cast_to_maybe_larger(const Source& a)
{
	CompileTimeAssert(sizeof(Target) >= sizeof(Source),
		"Aliasing type with a type of different size is not allowed");

	Target target{};
	std::memcpy(&target, &a, sizeof(Source));
	return target;
}