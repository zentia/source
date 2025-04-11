#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <map>
#include <set>
#else
#include <EASTL/vector_map.h>
#include <EASTL/vector_set.h>
#include <EASTL/vector_multimap.h>
#include <EASTL/vector_multiset.h>
#include <luisa/core/stl/functional.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL

template<typename Key, typename Value,
         typename Compare = std::less<>>
using vector_map = std::map<Key, Value, Compare>;

template<typename Key, typename Value,
         typename Compare = std::less<>>
using vector_multimap = std::multimap<Key, Value, Compare>;

template<typename Key,
         typename Compare = std::less<>>
using vector_set = std::set<Key, Compare>;

template<typename Key,
         typename Compare = std::less<>>
using vector_multiset = std::multiset<Key, Compare>;

#else

template<typename Key, typename Value,
         typename Compare = std::less<>>
using vector_map = eastl::vector_map<Key, Value, Compare>;

template<typename Key, typename Value,
         typename Compare = std::less<>>
using vector_multimap = eastl::vector_multimap<Key, Value, Compare>;

template<typename Key,
         typename Compare = std::less<>>
using vector_set = eastl::vector_set<Key, Compare>;

template<typename Key,
         typename Compare = std::less<>>
using vector_multiset = eastl::vector_multiset<Key, Compare>;

#endif

}// namespace luisa
