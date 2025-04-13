//
// Created by swfly on 2024/11/21.
//

#include <fstream>

#include <llvm/ExecutionEngine/Orc/LLJIT.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/IR/Verifier.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Analysis/AliasAnalysis.h>
#include <llvm/ExecutionEngine/ExecutionEngine.h>
#include <llvm/Analysis/CGSCCPassManager.h>
#include <llvm/Analysis/LoopAnalysisManager.h>
#include <llvm/Passes/PassBuilder.h>
#include <llvm/IR/LegacyPassManager.h>

#include <luisa/core/stl.h>
#include <luisa/core/logging.h>
#include <luisa/core/clock.h>

#include <luisa/xir/translators/ast2xir.h>
#include <luisa/xir/translators/xir2text.h>
#include <luisa/xir/instructions/print.h>

#include <luisa/xir/passes/dce.h>
#include <luisa/xir/passes/local_store_forward.h>
#include <luisa/xir/passes/local_load_elimination.h>
#include <luisa/xir/passes/mem2reg.h>
#include <luisa/xir/passes/lower_ray_query_loop.h>

#include "../common/shader_print_formatter.h"

#include "fallback_device.h"
#include "fallback_codegen.h"
#include "fallback_texture.h"
#include "fallback_accel.h"
#include "fallback_bindless_array.h"
#include "fallback_shader.h"
#include "fallback_buffer.h"
#include "fallback_command_queue.h"
#include "fallback_device_api.h"
#include "fallback_device_api_ir_module.h"

static const bool LUISA_SHOULD_DUMP_XIR = [] {
    if (auto env = getenv("LUISA_DUMP_XIR")) {
        return std::string_view{env} == "1";
    }
    return false;
}();

static const bool LUISA_SHOULD_DUMP_LLVM_IR = [] {
    if (auto env = getenv("LUISA_DUMP_LLVM_IR")) {
        return std::string_view{env} == "1";
    }
    return false;
}();

static const bool LUISA_SHOULD_DUMP_ASM = [] {
    if (auto env = getenv("LUISA_DUMP_ASM")) {
        return std::string_view{env} == "1";
    }
    return false;
}();

namespace luisa::compute::fallback {

[[nodiscard]] static luisa::half luisa_fallback_asin_f16(luisa::half x) noexcept { return ::half_float::asin(x); }
[[nodiscard]] static float luisa_fallback_asin_f32(float x) noexcept { return std::asin(x); }
[[nodiscard]] static double luisa_fallback_asin_f64(double x) noexcept { return std::asin(x); }

[[nodiscard]] static luisa::half luisa_fallback_acos_f16(luisa::half x) noexcept { return ::half_float::acos(x); }
[[nodiscard]] static float luisa_fallback_acos_f32(float x) noexcept { return std::acos(x); }
[[nodiscard]] static double luisa_fallback_acos_f64(double x) noexcept { return std::acos(x); }

[[nodiscard]] static luisa::half luisa_fallback_atan_f16(luisa::half x) noexcept { return ::half_float::atan(x); }
[[nodiscard]] static float luisa_fallback_atan_f32(float x) noexcept { return std::atan(x); }
[[nodiscard]] static double luisa_fallback_atan_f64(double x) noexcept { return std::atan(x); }

[[nodiscard]] static luisa::half luisa_fallback_atan2_f16(luisa::half a, luisa::half b) noexcept { return ::half_float::atan2(a, b); }
[[nodiscard]] static float luisa_fallback_atan2_f32(float a, float b) noexcept { return std::atan2(a, b); }
[[nodiscard]] static double luisa_fallback_atan2_f64(double a, double b) noexcept { return std::atan2(a, b); }

static void luisa_fallback_assert(bool condition, const char *message) noexcept {
    if (!condition) { LUISA_ERROR_WITH_LOCATION("Assertion failed: {}.", message); }
}

static thread_local const DeviceInterface::StreamLogCallback *current_device_log_callback{nullptr};

static void luisa_fallback_print(const FallbackShader *shader, size_t fmt_id, const std::byte *args) noexcept {
    static thread_local luisa::string scratch;
    scratch.clear();
    auto formatter = shader->print_formatter(fmt_id);
    (*formatter)(scratch, {args, formatter->size()});
    if (current_device_log_callback) {
        (*current_device_log_callback)(scratch);
    } else {
        LUISA_INFO("[DEVICE] {}", scratch);
    }
}

struct FallbackShaderLaunchConfig {
    uint3 block_id;
    uint3 dispatch_size;
    uint3 block_size;
};

FallbackShader::FallbackShader(FallbackDevice *device, const ShaderOption &option, Function kernel) noexcept {

    LUISA_VERBOSE("======= Fallback Backend JIT Shader Compilation =======");

    // build JIT engine
    ::llvm::orc::LLJITBuilder jit_builder;
    if (auto host = ::llvm::orc::JITTargetMachineBuilder::detectHost()) {
        ::llvm::TargetOptions options;
        if (option.enable_fast_math) {
            options.UnsafeFPMath = true;
            options.NoInfsFPMath = true;
            options.NoNaNsFPMath = true;
            options.NoSignedZerosFPMath = true;
            options.ApproxFuncFPMath = true;
        }
        options.NoTrappingFPMath = true;
        options.AllowFPOpFusion = ::llvm::FPOpFusion::Fast;
        options.EnableIPRA = false;// true causes crash
        options.StackSymbolOrdering = true;
#ifndef NDEBUG
        options.TrapUnreachable = true;
#else
        options.TrapUnreachable = false;
#endif
        options.EnableMachineFunctionSplitter = true;
        options.EnableMachineOutliner = false;
        options.NoTrapAfterNoreturn = true;
        host->setOptions(options);
        host->setCodeGenOptLevel(::llvm::CodeGenOptLevel::Aggressive);
#ifdef __aarch64__
        host->addFeatures({"+neon"});
#endif
        LUISA_VERBOSE("LLVM JIT target: triplet = {}, features = {}.",
                      host->getTargetTriple().str(), host->getFeatures().getString());
        if (auto machine = host->createTargetMachine()) {
            _target_machine = std::move(machine.get());
        } else {
            ::llvm::handleAllErrors(machine.takeError(), [&](const ::llvm::ErrorInfoBase &e) {
                LUISA_WARNING_WITH_LOCATION("JITTargetMachineBuilder::createTargetMachine(): {}.", e.message());
            });
            LUISA_ERROR_WITH_LOCATION("Failed to create target machine.");
        }
        jit_builder.setJITTargetMachineBuilder(std::move(*host));
    } else {
        ::llvm::handleAllErrors(host.takeError(), [&](const ::llvm::ErrorInfoBase &e) {
            LUISA_WARNING_WITH_LOCATION("JITTargetMachineBuilder::detectHost(): {}.", e.message());
        });
        LUISA_ERROR_WITH_LOCATION("Failed to detect host.");
    }

    if (auto expected_jit = jit_builder.create()) {
        _jit = std::move(expected_jit.get());
    } else {
        ::llvm::handleAllErrors(expected_jit.takeError(), [](const ::llvm::ErrorInfoBase &err) {
            LUISA_WARNING_WITH_LOCATION("LLJITBuilder::create(): {}", err.message());
        });
        LUISA_ERROR_WITH_LOCATION("Failed to create LLJIT.");
    }

    // if (auto generator = ::llvm::orc::DynamicLibrarySearchGenerator::GetForCurrentProcess(
    //         _jit->getDataLayout().getGlobalPrefix())) {
    //     _jit->getMainJITDylib().addGenerator(std::move(generator.get()));
    // } else {
    //     ::llvm::handleAllErrors(generator.takeError(), [](const ::llvm::ErrorInfoBase &err) {
    //         LUISA_WARNING_WITH_LOCATION("DynamicLibrarySearchGenerator::GetForCurrentProcess(): {}", err.message());
    //     });
    //     LUISA_ERROR_WITH_LOCATION("Failed to add generator.");
    // }

    _block_size = kernel.block_size();
    _build_bound_arguments(kernel.bound_arguments());

    Clock translate_clk;
    auto xir_module = xir::ast_to_xir_translate(kernel, {});
    xir_module->set_name(luisa::format("kernel_{:016x}", kernel.hash()));
    if (!option.name.empty()) { xir_module->set_location(option.name); }
    LUISA_VERBOSE("AST to XIR translation done in {} ms.", translate_clk.toc());

    // dump for debugging
    if (LUISA_SHOULD_DUMP_XIR) {
        auto filename = luisa::format("kernel.{:016x}.xir", kernel.hash());
        std::ofstream f{filename.c_str()};
        f << xir::xir_to_text_translate(xir_module.get(), true);
    }

    // run some simple optimization passes on XIR to reduce the size of LLVM IR
    Clock opt_clk;
    auto dce1_info = xir::dce_pass_run_on_module(xir_module.get());
    auto store_forward_info = xir::local_store_forward_pass_run_on_module(xir_module.get());
    auto load_elim_info = xir::local_load_elimination_pass_run_on_module(xir_module.get());
    auto dce2_info = xir::dce_pass_run_on_module(xir_module.get());
    auto mem2reg_info = xir::mem2reg_pass_run_on_module(xir_module.get());
    auto dce3_info = xir::dce_pass_run_on_module(xir_module.get());
    if (LUISA_SHOULD_DUMP_XIR) {
        auto filename = luisa::format("kernel.{:016x}.opt.xir", kernel.hash());
        std::ofstream f{filename.c_str()};
        f << xir::xir_to_text_translate(xir_module.get(), true);
    }
    auto rq_lower_info = xir::lower_ray_query_loop_pass_run_on_module(xir_module.get());
    LUISA_VERBOSE("XIR optimization done in {} ms: "
                  "forwarded {} store instruction(s), "
                  "eliminated {} load instruction(s), "
                  "promoted {} alloca instruction(s) with {} load and {} store instruction(s) removed and {} phi node(s) inserted, "
                  "removed {} + {} + {} = {} dead instruction(s), "
                  "lowered {} ray query loop(s).",
                  opt_clk.toc(),
                  store_forward_info.forwarded_instructions.size(),
                  load_elim_info.eliminated_instructions.size(),
                  mem2reg_info.promoted_alloca_instructions.size(), mem2reg_info.removed_load_instructions.size(), mem2reg_info.removed_store_instructions.size(), mem2reg_info.inserted_phi_instructions.size(),
                  dce1_info.removed_instructions.size(), dce2_info.removed_instructions.size(), dce3_info.removed_instructions.size(),
                  dce1_info.removed_instructions.size() + dce2_info.removed_instructions.size() + dce3_info.removed_instructions.size(),
                  rq_lower_info.lowered_loops.size());

    // dump for debugging
    if (LUISA_SHOULD_DUMP_XIR) {
        auto filename = luisa::format("kernel.{:016x}.opt.rq.xir", kernel.hash());
        std::ofstream f{filename.c_str()};
        f << xir::xir_to_text_translate(xir_module.get(), true);
    }

    auto llvm_ctx = std::make_unique<llvm::LLVMContext>();
    auto builtin_module = fallback_backend_device_builtin_module();
    llvm::SMDiagnostic parse_error;
    auto llvm_module = llvm::parseIR(llvm::MemoryBufferRef{builtin_module, ""}, parse_error, *llvm_ctx);
    if (!llvm_module) {
        LUISA_ERROR_WITH_LOCATION("Failed to generate LLVM IR: {}.",
                                  luisa::string_view{parse_error.getMessage()});
    }

    Clock codegen_clk;
    auto codegen_feedback = luisa_fallback_backend_codegen(*llvm_ctx, llvm_module.get(), xir_module.get());
    LUISA_VERBOSE("XIR to LLVM IR code generation done in {} ms.", codegen_clk.toc());

    if (llvm::verifyModule(*llvm_module, &llvm::errs())) {
        auto filename = luisa::format("kernel.{:016x}.err.ll", kernel.hash());
        std::error_code ec;
        llvm::raw_fd_ostream ofs{llvm::StringRef{filename}, ec};
        if (ec) {
            LUISA_ERROR_WITH_LOCATION("LLVM module verification failed. Failed to open file for dumping LLVM IR: {}.", ec.message());
        }
        llvm_module->print(ofs, nullptr, false, true);
        LUISA_ERROR_WITH_LOCATION("LLVM module verification failed. IR dumped to '{}'.", filename);
    }

    // map symbols
    llvm::orc::SymbolMap symbol_map{};
    auto map_symbol = [jit = _jit.get(), &symbol_map]<typename T>(const char *name, T *f) noexcept {
        auto addr = llvm::orc::ExecutorAddr::fromPtr(f);
        auto symbol = llvm::orc::ExecutorSymbolDef{addr, llvm::JITSymbolFlags::Callable};
        symbol_map.try_emplace(jit->mangleAndIntern(name), symbol);
    };

#include "fallback_device_api_map_symbols.inl.h"

    // asin, acos, atan, atan2
    map_symbol("luisa.asin.f16", &luisa_fallback_asin_f16);
    map_symbol("luisa.asin.f32", &luisa_fallback_asin_f32);
    map_symbol("luisa.asin.f64", &luisa_fallback_asin_f64);
    map_symbol("luisa.acos.f16", &luisa_fallback_acos_f16);
    map_symbol("luisa.acos.f32", &luisa_fallback_acos_f32);
    map_symbol("luisa.acos.f64", &luisa_fallback_acos_f64);
    map_symbol("luisa.atan.f16", &luisa_fallback_atan_f16);
    map_symbol("luisa.atan.f32", &luisa_fallback_atan_f32);
    map_symbol("luisa.atan.f64", &luisa_fallback_atan_f64);
    map_symbol("luisa.atan2.f16", &luisa_fallback_atan2_f16);
    map_symbol("luisa.atan2.f32", &luisa_fallback_atan2_f32);
    map_symbol("luisa.atan2.f64", &luisa_fallback_atan2_f64);

    // assert
    map_symbol("luisa.assert", &luisa_fallback_assert);

    // bind print instructions
    if (!codegen_feedback.print_inst_map.empty()) {
        map_symbol("luisa.print.context", this);
        _print_formatters.reserve(codegen_feedback.print_inst_map.size());
        for (auto fmt_id = 0u; fmt_id < codegen_feedback.print_inst_map.size(); fmt_id++) {
            auto &&[print_inst, llvm_symbol] = codegen_feedback.print_inst_map[fmt_id];
            map_symbol(llvm_symbol.c_str(), &luisa_fallback_print);
            LUISA_INFO("Mapping print instruction #{}: \"{}\" -> {}", fmt_id, print_inst->format(), llvm_symbol);
            llvm::SmallVector<const Type *, 8u> arg_types;
            for (auto o : print_inst->operand_uses()) {
                arg_types.emplace_back(o->value()->type());
            }
            auto arg_pack_type = Type::structure(16u, arg_types);
            _print_formatters.emplace_back(luisa::make_unique<ShaderPrintFormatter>(
                print_inst->format(), arg_pack_type, false));
        }
    }

    // define symbols
    if (auto error = _jit->getMainJITDylib().define(
            ::llvm::orc::absoluteSymbols(std::move(symbol_map)))) {
        ::llvm::handleAllErrors(std::move(error), [](const ::llvm::ErrorInfoBase &err) {
            LUISA_WARNING_WITH_LOCATION("LLJIT::define(): {}", err.message());
        });
        LUISA_ERROR_WITH_LOCATION("Failed to define symbols.");
    }

    llvm_module->setDataLayout(_target_machine->createDataLayout());
    llvm_module->setTargetTriple(_target_machine->getTargetTriple().str());

    // add fast-math flags to instructions
    for (auto &&f : *llvm_module) {
        for (auto &&bb : f) {
            for (auto &&inst : bb) {
                if (llvm::isa<llvm::FPMathOperator>(inst)) {
                    inst.setFast(option.enable_fast_math);
                }
            }
        }
    }

    if (LUISA_SHOULD_DUMP_LLVM_IR) {
        auto filename = luisa::format("kernel.{:016x}.ll", kernel.hash());
        std::error_code ec;
        llvm::raw_fd_ostream ofs{llvm::StringRef{filename}, ec};
        if (ec) {
            LUISA_WARNING_WITH_LOCATION("Failed to open file for dumping LLVM IR: {}.", ec.message());
        } else {
            llvm_module->print(ofs, nullptr, false, true);
        }
    }

    // optimize
    ::llvm::LoopAnalysisManager LAM;
    ::llvm::FunctionAnalysisManager FAM;
    ::llvm::CGSCCAnalysisManager CGAM;
    ::llvm::ModuleAnalysisManager MAM;
    ::llvm::PipelineTuningOptions PTO;
    PTO.LoopInterleaving = true;
    PTO.LoopVectorization = true;
    PTO.SLPVectorization = true;
    PTO.LoopUnrolling = true;
    PTO.MergeFunctions = true;
    ::llvm::PassBuilder PB{_target_machine.get(), PTO};
    FAM.registerPass([&] { return PB.buildDefaultAAPipeline(); });
    PB.registerModuleAnalyses(MAM);
    PB.registerCGSCCAnalyses(CGAM);
    PB.registerFunctionAnalyses(FAM);
    PB.registerLoopAnalyses(LAM);
    PB.crossRegisterProxies(LAM, FAM, CGAM, MAM);
#if LLVM_VERSION_MAJOR >= 19
    _target_machine->registerPassBuilderCallbacks(PB);
#else
    _target_machine->registerPassBuilderCallbacks(PB, true);
#endif
    Clock clk;
    clk.tic();
    auto MPM = PB.buildPerModuleDefaultPipeline(::llvm::OptimizationLevel::O3);
    MPM.run(*llvm_module, MAM);

    LUISA_VERBOSE("Optimized LLVM module in {} ms.", clk.toc());
    if (::llvm::verifyModule(*llvm_module, &::llvm::errs())) {
        LUISA_ERROR_WITH_LOCATION("Failed to verify module.");
    }

    if (LUISA_SHOULD_DUMP_LLVM_IR) {
        auto filename = luisa::format("kernel.{:016x}.opt.ll", kernel.hash());
        std::error_code ec;
        llvm::raw_fd_ostream ofs{llvm::StringRef{filename}, ec};
        if (ec) {
            LUISA_WARNING_WITH_LOCATION("Failed to open file for dumping optimized LLVM IR: {}.", ec.message());
        } else {
            llvm_module->print(ofs, nullptr, false, true);
        }
    }

    if (LUISA_SHOULD_DUMP_ASM) {
        auto asm_name = luisa::format("kernel.{:016x}.s", kernel.hash());
        std::error_code ec;
        llvm::raw_fd_ostream ofs{llvm::StringRef{asm_name}, ec};
        if (ec) {
            LUISA_WARNING_WITH_LOCATION("Failed to open file for dumping assembly: {}.", ec.message());
        } else {
            llvm::legacy::PassManager pass;
            if (_target_machine->addPassesToEmitFile(pass, ofs, nullptr, llvm::CodeGenFileType::AssemblyFile)) {
                LUISA_ERROR_WITH_LOCATION("TheTargetMachine can't emit a file of this type");
            }
            pass.run(*llvm_module);
        }
    }

    // compile to machine code
    auto m = llvm::orc::ThreadSafeModule(std::move(llvm_module), std::move(llvm_ctx));
    if (auto error = _jit->addIRModule(std::move(m))) {
        ::llvm::handleAllErrors(std::move(error), [](const ::llvm::ErrorInfoBase &err) {
            LUISA_WARNING_WITH_LOCATION("LLJIT::addIRModule(): {}", err.message());
        });
    }
    auto addr = _jit->lookup("kernel.main");
    if (!addr) {
        ::llvm::handleAllErrors(addr.takeError(), [](const ::llvm::ErrorInfoBase &err) {
            LUISA_WARNING_WITH_LOCATION("LLJIT::lookup(): {}", err.message());
        });
    }
    LUISA_ASSERT(addr, "JIT compilation failed.");
    _kernel_entry = addr->toPtr<kernel_entry_t>();

    // compute argument buffer size
    _argument_buffer_size = 0u;
    static constexpr auto argument_alignment = 16u;
    for (auto arg : kernel.arguments()) {
        switch (arg.tag()) {
            case Variable::Tag::LOCAL: {
                _argument_buffer_size += arg.type()->size();
                _argument_buffer_size = luisa::align(_argument_buffer_size, argument_alignment);
                break;
            }
            case Variable::Tag::BUFFER: {
                _argument_buffer_size += sizeof(FallbackBufferView);
                _argument_buffer_size = luisa::align(_argument_buffer_size, argument_alignment);
                break;
            }
            case Variable::Tag::TEXTURE: {
                _argument_buffer_size += sizeof(FallbackTextureView);
                _argument_buffer_size = luisa::align(_argument_buffer_size, argument_alignment);
                break;
            }
            case Variable::Tag::BINDLESS_ARRAY: {
                _argument_buffer_size += sizeof(FallbackBindlessArray *);
                _argument_buffer_size = luisa::align(_argument_buffer_size, argument_alignment);
                break;
            }
            case Variable::Tag::ACCEL: {
                _argument_buffer_size += sizeof(FallbackAccel *);
                _argument_buffer_size = luisa::align(_argument_buffer_size, argument_alignment);
                break;
            }
            default: LUISA_ERROR_WITH_LOCATION("Unsupported argument type.");
        }
    }
}

class FallbackShaderDispatchBuffer {

public:
    struct alignas(16) Config {
        FallbackShader::kernel_entry_t *kernel;
        std::array<uint, 3> dispatch_size;
        std::array<uint, 3> block_size;
    };

private:
    static constexpr auto argument_buffer_offset = sizeof(Config);// grid size
    std::byte *_data;

public:
    explicit FallbackShaderDispatchBuffer(size_t size) noexcept
        : _data{luisa::allocate_with_allocator<std::byte>(argument_buffer_offset + size)} {}
    ~FallbackShaderDispatchBuffer() noexcept {
        if (_data != nullptr) {
            luisa::deallocate_with_allocator(_data);
        }
    }
    FallbackShaderDispatchBuffer(FallbackShaderDispatchBuffer &&other) noexcept
        : _data{std::exchange(other._data, nullptr)} {}
    FallbackShaderDispatchBuffer(const FallbackShaderDispatchBuffer &) = delete;
    FallbackShaderDispatchBuffer &operator=(FallbackShaderDispatchBuffer &&) noexcept = delete;
    FallbackShaderDispatchBuffer &operator=(const FallbackShaderDispatchBuffer &) = delete;
    [[nodiscard]] auto argument_buffer() noexcept { return _data + argument_buffer_offset; }
    [[nodiscard]] auto argument_buffer() const noexcept { return const_cast<FallbackShaderDispatchBuffer *>(this)->argument_buffer(); }
    [[nodiscard]] auto config() noexcept { return reinterpret_cast<Config *>(_data); }
    [[nodiscard]] auto config() const noexcept { return const_cast<FallbackShaderDispatchBuffer *>(this)->config(); }
};

void FallbackShader::dispatch(FallbackCommandQueue *queue, luisa::unique_ptr<ShaderDispatchCommand> command) noexcept {

    auto dispatch_size = command->dispatch_size();
    auto block_size = _block_size;

    FallbackShaderDispatchBuffer dispatch_buffer{_argument_buffer_size};
    auto dispatch_config = dispatch_buffer.config();
    dispatch_config->kernel = _kernel_entry;
    dispatch_config->dispatch_size = {dispatch_size.x, dispatch_size.y, dispatch_size.z};
    dispatch_config->block_size = {block_size.x, block_size.y, block_size.z};

    auto argument_buffer = dispatch_buffer.argument_buffer();
    auto argument_buffer_offset = static_cast<size_t>(0u);
    auto allocate_argument = [&](size_t bytes) noexcept {
        static constexpr auto alignment = 16u;
        auto offset = (argument_buffer_offset + alignment - 1u) / alignment * alignment;
        LUISA_ASSERT(offset + bytes <= _argument_buffer_size,
                     "Too many arguments in ShaderDispatchCommand");
        argument_buffer_offset = offset + bytes;
        return argument_buffer + offset;
    };

    auto encode_argument = [&allocate_argument, &command](const auto &arg) noexcept {
        using Tag = ShaderDispatchCommand::Argument::Tag;
        switch (arg.tag) {
            case Tag::BUFFER: {
                auto buffer = reinterpret_cast<FallbackBuffer *>(arg.buffer.handle);
                auto buffer_view = buffer->view(arg.buffer.offset, arg.buffer.size);
                auto ptr = allocate_argument(sizeof(buffer_view));
                std::memcpy(ptr, &buffer_view, sizeof(buffer_view));
                break;
            }
            case Tag::TEXTURE: {
                auto texture = reinterpret_cast<const FallbackTexture *>(arg.texture.handle);
                auto view = texture->view(arg.texture.level);
                auto ptr = allocate_argument(sizeof(view));
                std::memcpy(ptr, &view, sizeof(view));
                break;
            }
            case Tag::UNIFORM: {
                auto uniform = command->uniform(arg.uniform);
                auto ptr = allocate_argument(uniform.size_bytes());
                std::memcpy(ptr, uniform.data(), uniform.size_bytes());
                break;
            }
            case Tag::BINDLESS_ARRAY: {
                auto bindless = reinterpret_cast<FallbackBindlessArray *>(arg.bindless_array.handle);
                auto view = bindless->view();
                auto ptr = allocate_argument(sizeof(view));
                std::memcpy(ptr, &view, sizeof(view));
                break;
            }
            case Tag::ACCEL: {
                auto accel = reinterpret_cast<FallbackAccel *>(arg.accel.handle);
                auto view = accel->view();
                auto ptr = allocate_argument(sizeof(view));
                std::memcpy(ptr, &view, sizeof(view));
                break;
            }
            default: LUISA_ERROR_WITH_LOCATION("Unsupported argument type.");
        }
    };
    for (auto &&arg : _bound_arguments) { encode_argument(arg); }
    for (auto &&arg : command->arguments()) { encode_argument(arg); }

    static constexpr auto roundup_div = [](auto a, auto b) noexcept {
        return (a + b - 1u) / b;
    };

    auto grid_size = roundup_div(dispatch_size, block_size);
    auto grid_count = grid_size.x * grid_size.y * grid_size.z;

    queue->enqueue_parallel(grid_count, [queue, dispatch_buffer = std::move(dispatch_buffer)](auto block) noexcept {
        auto config = dispatch_buffer.config();
        auto dispatch_size = config->dispatch_size;
        auto block_size = config->block_size;
        auto grid_size_x = roundup_div(dispatch_size[0], block_size[0]);
        auto grid_size_y = roundup_div(dispatch_size[1], block_size[1]);
        auto bx = block % grid_size_x;
        auto by = (block / grid_size_x) % grid_size_y;
        auto bz = block / (grid_size_x * grid_size_y);
        FallbackShaderLaunchConfig launch_config{
            .block_id = make_uint3(bx, by, bz),
            .dispatch_size = {dispatch_size[0], dispatch_size[1], dispatch_size[2]},
            .block_size = {block_size[0], block_size[1], block_size[2]},
        };
        auto launch_params = dispatch_buffer.argument_buffer();
        current_device_log_callback = queue->log_callback() ? &queue->log_callback() : nullptr;
        config->kernel(launch_params, &launch_config);
        current_device_log_callback = nullptr;
    });
}

FallbackShader::~FallbackShader() noexcept = default;

void FallbackShader::_build_bound_arguments(luisa::span<const Function::Binding> bindings) noexcept {
    _bound_arguments.reserve(bindings.size());
    for (auto &&arg : bindings) {
        luisa::visit(
            [&]<typename T>(T binding) noexcept {
                ShaderDispatchCommand::Argument argument{};
                if constexpr (std::is_same_v<T, Function::BufferBinding>) {
                    argument.tag = ShaderDispatchCommand::Argument::Tag::BUFFER;
                    argument.buffer.handle = binding.handle;
                    argument.buffer.offset = binding.offset;
                    argument.buffer.size = binding.size;
                } else if constexpr (std::is_same_v<T, Function::TextureBinding>) {
                    argument.tag = ShaderDispatchCommand::Argument::Tag::TEXTURE;
                    argument.texture.handle = binding.handle;
                    argument.texture.level = binding.level;
                } else if constexpr (std::is_same_v<T, Function::BindlessArrayBinding>) {
                    argument.tag = ShaderDispatchCommand::Argument::Tag::BINDLESS_ARRAY;
                    argument.bindless_array.handle = binding.handle;
                } else if constexpr (std::is_same_v<T, Function::AccelBinding>) {
                    argument.tag = ShaderDispatchCommand::Argument::Tag::ACCEL;
                    argument.accel.handle = binding.handle;
                } else {
                    LUISA_ERROR_WITH_LOCATION("Unsupported binding type.");
                }
                _bound_arguments.emplace_back(argument);
            },
            arg);
    }
}

}// namespace luisa::compute::fallback
