#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <algorithm>
#else
#include <EASTL/algorithm.h>
#endif

#include <runtime/core/stl/pdqsort.h>

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL
using std::transform;
using std::swap;
using std::binary_search;
#else
using eastl::transform;
using eastl::swap;
using eastl::binary_search;
#endif
template<pdqsort_detail::LinearIterable Iter>
inline void sort(Iter begin, Iter end) {
    pdqsort(begin, end);
}

template<pdqsort_detail::LinearIterable Iter, pdqsort_detail::CompareFunc<Iter> Compare>
inline void sort(Iter begin, Iter end, Compare comp) {
    pdqsort(begin, end, comp);
}

}// namespace luisa
