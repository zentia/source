//
// Created by Mike on 8/29/2023.
//

#include <runtime/core/dll_export.h>
#include <runtime/core/stl/string.h>
#include <runtime/ast/function.h>

namespace luisa::compute {
[[nodiscard]] LC_AST_API luisa::string to_json(const Type *type) noexcept;
[[nodiscard]] LC_AST_API luisa::string to_json(Function function) noexcept;
}// namespace luisa::compute
