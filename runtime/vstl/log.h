#pragma once
#include <runtime/vstl/config.h>
#include <runtime/vstl/vstring.h>
#include <initializer_list>
LC_VSTL_API void vengine_log(std::string_view const &chunk);
LC_VSTL_API void vengine_log(std::string_view const *chunk, size_t chunkCount);
LC_VSTL_API void vengine_log(std::initializer_list<std::string_view> const &initList);
LC_VSTL_API void vengine_log(char const *chunk);
