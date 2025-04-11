#pragma once
#include <luisa/core/dynamic_module.h>
#include <luisa/core/first_fit.h>
#include <luisa/ast/type.h>
#include <luisa/ast/function_builder.h>
namespace luisa {
class MemorySanitizer {
    MemorySanitizer() = delete;
    MemorySanitizer(MemorySanitizer const &) = delete;
    MemorySanitizer(MemorySanitizer &&) = delete;
    MemorySanitizer &operator=(MemorySanitizer const &) = delete;
    MemorySanitizer &operator=(MemorySanitizer &&) = delete;
    ~MemorySanitizer() = delete;
public:
    static void reset_static_vars() {
        ::luisa::DynamicModule::reset_search_paths();
        ::luisa::compute::Type::reset_type_registry();
        ::luisa::compute::detail::FunctionBuilder::_function_stack() = luisa::vector<::luisa::compute::detail::FunctionBuilder *>{};
        ::luisa::FirstFit::dispose_first_fit_node_pool();
    }
};
}// namespace luisa