#pragma once

#include <runtime/core/stl/optional.h>

#ifdef LUISA_USE_SYSTEM_STL
#include <variant>
#else
#include <EASTL/variant.h>
#endif

namespace luisa {

#ifdef LUISA_USE_SYSTEM_STL
using std::get;
using std::get_if;
using std::holds_alternative;
using std::monostate;
using std::variant;
using std::variant_alternative_t;
using std::variant_size_v;
using std::visit;
#else
using eastl::get;
using eastl::get_if;
using eastl::holds_alternative;
using eastl::monostate;
using eastl::variant;
using eastl::variant_alternative_t;
using eastl::variant_size_v;
using eastl::visit;
#endif

}// namespace luisa
