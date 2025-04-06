#pragma once

#if ENABLE_CPP_RTTI
#include <typeinfo>
#endif

template<bool B, class T, class F>
struct Conditional { typedef T type; };

template<class T, class F>
struct Conditional<false, T, F> { typedef F type; };

template<bool B, class T = void>
struct EnableIf {};

template<class T>
struct EnableIf<true, T> { typedef T type; };

struct NullType {};