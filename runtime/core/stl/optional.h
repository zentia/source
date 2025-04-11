#pragma once

#ifdef LUISA_USE_SYSTEM_STL
#include <optional>
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

#include <EASTL/optional.h>

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
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;
#else
using eastl::make_optional;
using eastl::nullopt;
using eastl::nullopt_t;
using eastl::optional;
#endif

}// namespace luisa
