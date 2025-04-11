#pragma once

#ifndef LUISA_USE_SYSTEM_STL

#include <EASTL/bonus/ring_buffer.h>
#include <EASTL/bonus/fixed_ring_buffer.h>

namespace luisa {

template<typename T>
using ring_buffer = eastl::ring_buffer<T>;

template<typename T, size_t N>
using fixed_ring_buffer = eastl::fixed_ring_buffer<T, N>;

}// namespace luisa

#endif
