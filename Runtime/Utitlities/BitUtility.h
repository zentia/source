#pragma once

#include <limits.h>
#include "baselib/Include/Cpp/Algorithm.h"

#if _MSC_VER

#endif

// �������λΪ1��λ��
inline int HighestBit(UInt32 mask)
{
	if (mask == 0)
		return -1;
#if _MSC_VER
	unsigned long res = 0;
	_BitScanReverse(&res, mask);
	return (int)res;
#endif

}