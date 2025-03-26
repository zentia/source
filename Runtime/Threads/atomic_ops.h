#pragma once

#if PLATFORM_WIN
#include<windef.h>
#endif

inline int atomic_add(int volatile* i, const int value)
{
	return _InterlockedExchangeAdd(reinterpret_cast<long volatile*>(i), value) + value;
}

inline bool atomic_compare_exchange(int volatile* i, const int new_value, const int expected_value)
{
	return _InterlockedCompareExchange(reinterpret_cast<long volatile*>(i), new_value, expected_value) == expected_value;
}