#pragma once

#ifdef LUISA_USE_SYSTEM_STL

#include <span>
#include <vector>

#else

#if defined(__clang__)
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
#elif defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
#elif defined(_MSC_VER)
#pragma warning(push)
#pragma warning(disable : 4996)
#endif

#include <EASTL/vector.h>
#include <EASTL/fixed_vector.h>
#include <EASTL/bitvector.h>

#include <runtime/core/stl/type_traits.h>

#if defined(__clang__)
#pragma clang diagnostic pop
#elif defined(__GNUC__)
#pragma GCC diagnostic pop
#elif defined(_MSC_VER)
#pragma warning(pop)
#endif

#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL

using std::vector;
using bitvector = std::vector<bool>;

// FIXME: we should implement an efficient fixed_vector
template<typename T, size_t node_count, bool allow_overflow = true>
using fixed_vector = std::vector<T>;

#else

template<typename T>
using vector = eastl::vector<T>;

template<typename T, size_t node_count, bool allow_overflow = true>
using fixed_vector = eastl::fixed_vector<T, node_count, allow_overflow>;

using bitvector = eastl::bitvector<>;

#endif

#ifdef LUISA_USE_SYSTEM_STL
template<typename... T>
auto enlarge_by(luisa::vector<T...> &vec, size_t size) noexcept {
    auto old_size = vec.size();
    vec.resize(old_size + size);
    return vec.data() + old_size;
}
template<typename... T>
auto size_bytes(const luisa::vector<T...> &vec) noexcept {
    return std::span{vec}.size_bytes();
}
template<typename T>
auto vector_resize(luisa::vector<T> &vec, size_t size) noexcept {
    return vec.resize(size);
}
#else
template<typename T>
auto enlarge_by(luisa::vector<T> &vec, size_t size) noexcept {
    return vec.push_back_uninitialized(size);
}
template<typename T, size_t node_count, bool allow_overflow>
auto enlarge_by(luisa::fixed_vector<T, node_count, allow_overflow> &vec, size_t size) noexcept {
    return vec.push_back_uninitialized(size);
}

template<typename T>
auto vector_resize(luisa::vector<T> &vec, size_t size) noexcept {
    return vec.resize_uninitialized(size);
}
template<typename T, size_t node_count, bool allow_overflow>
auto vector_resize(luisa::fixed_vector<T, node_count, allow_overflow> &vec, size_t size) noexcept {
    return vec.resize_uninitialized(size);
}

template<typename T>
auto size_bytes(const luisa::vector<T> &vec) noexcept {
    return vec.size_bytes();
}
template<typename T, size_t node_count, bool allow_overflow>
auto size_bytes(const luisa::fixed_vector<T, node_count, allow_overflow> &vec) noexcept {
    return vec.size_bytes();
}
#endif

}// namespace luisa
