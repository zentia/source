#pragma once

#include <luisa/core/stl/unordered_map.h>

namespace llvm {
class Module;
class LLVMContext;
}// namespace llvm

namespace luisa::compute::xir {
class PrintInst;
class Module;
}// namespace luisa::compute::xir

namespace luisa::compute::fallback {

struct FallbackCodeGenFeedback {
    using PrintInstMap = luisa::vector<std::pair<
        const xir::PrintInst *,
        luisa::string /* llvm symbol */>>;
    PrintInstMap print_inst_map;
};

[[nodiscard]] FallbackCodeGenFeedback
luisa_fallback_backend_codegen(llvm::LLVMContext &llvm_ctx,
                               llvm::Module *llvm_module,
                               const xir::Module *module) noexcept;

}// namespace luisa::compute::fallback
