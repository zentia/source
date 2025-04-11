#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <deque>
#else
#include <EASTL/deque.h>
#endif

namespace luisa {
#ifdef LUISA_USE_SYSTEM_STL
using std::deque;
#else
template<typename T>
using deque = eastl::deque<T>;
#endif
}// namespace luisa
