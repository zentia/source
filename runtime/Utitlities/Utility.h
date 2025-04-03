#pragma once

template<class T>
inline T clamp(const T& t, const T& t0, const T& t1)
{
	if (t < t0)
		return t0;
	if (t > t1)
		return t1;
	return t;
}