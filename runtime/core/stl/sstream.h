#pragma once

#include <luisa/core/stl/string.h>
#include <sstream>

namespace luisa {
#ifdef LUISA_USE_SYSTEM_STL
using std::stringstream;
using std::istringstream;
using std::ostringstream;
#else
using stringstream = std::basic_stringstream<char, std::char_traits<char>, luisa::allocator<char>>;
using istringstream = std::basic_istringstream<char, std::char_traits<char>, luisa::allocator<char>>;
using ostringstream = std::basic_ostringstream<char, std::char_traits<char>, luisa::allocator<char>>;
#endif
}// namespace luisa