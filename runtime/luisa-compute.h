#pragma once

#include <runtime/ast/ast2json.h>
#include <runtime/ast/atomic_ref_node.h>
#include <runtime/ast/attribute.h>
#include <runtime/ast/callable_library.h>
#include <runtime/ast/constant_data.h>
#include <runtime/ast/expression.h>
#include <runtime/ast/external_function.h>
#include <runtime/ast/function.h>
#include <runtime/ast/function_builder.h>
#include <runtime/ast/interface.h>
#include <runtime/ast/op.h>
#include <runtime/ast/statement.h>
#include <runtime/ast/type.h>
#include <runtime/ast/type_registry.h>
#include <runtime/ast/usage.h>
#include <runtime/ast/variable.h>

#ifdef LUISA_ENABLE_CLANGCXX
#include <runtime/clangcxx/ast.h>
#include <runtime/clangcxx/compiler.h>
#endif

#include <runtime/core/basic_traits.h>
#include <runtime/core/basic_types.h>
#include <runtime/core/binary_buffer.h>
#include <runtime/core/binary_file_stream.h>
#include <runtime/core/binary_io.h>
#include <runtime/core/clock.h>
#include <runtime/core/concepts.h>
#include <runtime/core/constants.h>
#include <runtime/core/dll_export.h>
#include <runtime/core/dynamic_module.h>
#include <runtime/core/fiber.h>
#include <runtime/core/first_fit.h>
#include <runtime/core/forget.h>
#include <runtime/core/intrin.h>
#include <runtime/core/logging.h>
#include <runtime/core/macro.h>
#include <runtime/core/magic_enum.h>
#include <runtime/core/mathematics.h>
#include <runtime/core/memory_sanitizer.hpp>
#include <runtime/core/platform.h>
#include <runtime/core/pool.h>
#include <runtime/core/shared_function.h>
#include <runtime/core/spin_mutex.h>
#include <runtime/core/stl.h>
#include <runtime/core/string_scratch.h>
#include <runtime/core/thread_safety.h>

#ifdef LUISA_ENABLE_DSL
#include <runtime/dsl/arg.h>
#include <runtime/dsl/atomic.h>
#include <runtime/dsl/binding_group.h>
#include <runtime/dsl/builtin.h>
#include <runtime/dsl/callable_library.h>
#include <runtime/dsl/constant.h>
#include <runtime/dsl/dispatch_indirect.h>
#include <runtime/dsl/expr.h>
#include <runtime/dsl/expr_traits.h>
#include <runtime/dsl/func.h>
#include <runtime/dsl/local.h>
#include <runtime/dsl/operators.h>
#include <runtime/dsl/polymorphic.h>
#include <runtime/dsl/raster/raster_func.h>
#include <runtime/dsl/raster/raster_kernel.h>
#include <runtime/dsl/ref.h>
#include <runtime/dsl/resource.h>
#include <runtime/dsl/rtx/aabb.h>
#include <runtime/dsl/rtx/accel.h>
#include <runtime/dsl/rtx/curve.h>
#include <runtime/dsl/rtx/hit.h>
#include <runtime/dsl/rtx/motion.h>
#include <runtime/dsl/rtx/ray.h>
#include <runtime/dsl/rtx/ray_query.h>
#include <runtime/dsl/rtx/triangle.h>
#include <runtime/dsl/shared.h>
#include <runtime/dsl/soa.h>
#include <runtime/dsl/stmt.h>
#include <runtime/dsl/struct.h>
#include <runtime/dsl/sugar.h>
#include <runtime/dsl/syntax.h>
#include <runtime/dsl/var.h>
#endif

#ifdef LUISA_ENABLE_GUI
#include <runtime/gui/framerate.h>
#include <runtime/gui/imgui_window.h>
#include <runtime/gui/input.h>
#include <runtime/gui/window.h>
#endif

#ifdef LUISA_ENABLE_IR
#include <runtime/ir/ast2ir.h>
#include <runtime/ir/fwd.h>
#include <runtime/ir/ir2ast.h>
#include <runtime/ir/transform.h>
#endif

#include <runtime/osl/hint.h>
#include <runtime/osl/instruction.h>
#include <runtime/osl/literal.h>
#include <runtime/osl/oso_parser.h>
#include <runtime/osl/shader.h>
#include <runtime/osl/symbol.h>
#include <runtime/osl/type.h>

#include <runtime/runtime/bindless_array.h>
#include <runtime/runtime/buffer.h>
#include <runtime/runtime/buffer_arena.h>
#include <runtime/runtime/byte_buffer.h>
#include <runtime/runtime/command_list.h>
#include <runtime/runtime/context.h>
#include <runtime/runtime/depth_format.h>
#include <runtime/runtime/device.h>
#include <runtime/runtime/dispatch_buffer.h>
#include <runtime/runtime/event.h>
#include <runtime/runtime/image.h>
#include <runtime/runtime/mipmap.h>
#include <runtime/runtime/raster/app_data.h>
#include <runtime/runtime/raster/depth_buffer.h>
#include <runtime/runtime/raster/raster_scene.h>
#include <runtime/runtime/raster/raster_shader.h>
#include <runtime/runtime/raster/raster_state.h>
#include <runtime/runtime/raster/vertex_attribute.h>
#include <runtime/runtime/raster/viewport.h>
#include <runtime/runtime/remote/client_interface.h>
#include <runtime/runtime/remote/server_interface.h>
#include <runtime/runtime/rhi/argument.h>
#include <runtime/runtime/rhi/command.h>
#include <runtime/runtime/rhi/command_encoder.h>
#include <runtime/runtime/rhi/curve_basis.h>
#include <runtime/runtime/rhi/device_interface.h>
#include <runtime/runtime/rhi/pixel.h>
#include <runtime/runtime/rhi/resource.h>
#include <runtime/runtime/rhi/sampler.h>
#include <runtime/runtime/rhi/stream_tag.h>
#include <runtime/runtime/rhi/tile_modification.h>
#include <runtime/runtime/rtx/aabb.h>
#include <runtime/runtime/rtx/accel.h>
#include <runtime/runtime/rtx/curve.h>
#include <runtime/runtime/rtx/hit.h>
#include <runtime/runtime/rtx/mesh.h>
#include <runtime/runtime/rtx/motion_instance.h>
#include <runtime/runtime/rtx/motion_transform.h>
#include <runtime/runtime/rtx/procedural_primitive.h>
#include <runtime/runtime/rtx/ray.h>
#include <runtime/runtime/rtx/triangle.h>
#include <runtime/runtime/shader.h>
#include <runtime/runtime/sparse_buffer.h>
#include <runtime/runtime/sparse_command_list.h>
#include <runtime/runtime/sparse_heap.h>
#include <runtime/runtime/sparse_image.h>
#include <runtime/runtime/sparse_texture.h>
#include <runtime/runtime/sparse_volume.h>
#include <runtime/runtime/stream.h>
#include <runtime/runtime/stream_event.h>
#include <runtime/runtime/swapchain.h>
#include <runtime/runtime/volume.h>

#ifdef LUISA_ENABLE_RUST
#include <runtime/rust/api_types.h>
#include <runtime/rust/api_types.hpp>
#include <runtime/rust/ir.hpp>
#include <runtime/rust/ir_common.h>
#endif

#ifdef LUISA_ENABLE_TENSOR
#include <luisa/tensor/expression.h>
#include <luisa/tensor/fused_activation.h>
#include <luisa/tensor/kernel.h>
#include <luisa/tensor/pass/expr_topo.h>
#include <runtime/tensor/tensor.h>
#include <runtime/tensor/tensor_builder.h>
#endif

#include <runtime/vstl/allocate_type.h>
#include <runtime/vstl/arena_hash_map.h>
#include <runtime/vstl/common.h>
#include <runtime/vstl/compare.h>
#include <runtime/vstl/config.h>
#include <runtime/vstl/functional.h>
#include <runtime/vstl/hash.h>
#include <runtime/vstl/hash_map.h>
#include <runtime/vstl/lmdb.hpp>
#include <runtime/vstl/lockfree_array_queue.h>
#include <runtime/vstl/log.h>
#include <runtime/vstl/md5.h>
#include <runtime/vstl/memory.h>
#include <runtime/vstl/meta_lib.h>
#include <runtime/vstl/pool.h>
#include <runtime/vstl/ranges.h>
#include <runtime/vstl/spin_mutex.h>
#include <runtime/vstl/stack_allocator.h>
#include <runtime/vstl/string_hash.h>
#include <runtime/vstl/string_utility.h>
#include <runtime/vstl/tree_map_base.h>
#include <runtime/vstl/unique_ptr.h>
#include <runtime/vstl/v_allocator.h>
#include <runtime/vstl/v_guid.h>
#include <runtime/vstl/vector.h>
#include <runtime/vstl/vstring.h>

#ifdef LUISA_ENABLE_XIR
#include <runtime/xir/argument.h>
#include <runtime/xir/basic_block.h>
#include <runtime/xir/builder.h>
#include <runtime/xir/constant.h>
#include <runtime/xir/debug_printer.h>
#include <runtime/xir/function.h>
#include <runtime/xir/ilist.h>
#include <runtime/xir/instruction.h>
#include <runtime/xir/instructions/alloca.h>
#include <runtime/xir/instructions/arithmetic.h>
#include <runtime/xir/instructions/assert.h>
#include <runtime/xir/instructions/assume.h>
#include <runtime/xir/instructions/atomic.h>
#include <runtime/xir/instructions/autodiff.h>
#include <runtime/xir/instructions/branch.h>
#include <runtime/xir/instructions/break.h>
#include <runtime/xir/instructions/call.h>
#include <runtime/xir/instructions/cast.h>
#include <runtime/xir/instructions/clock.h>
#include <runtime/xir/instructions/continue.h>
#include <runtime/xir/instructions/gep.h>
#include <runtime/xir/instructions/if.h>
#include <runtime/xir/instructions/load.h>
#include <runtime/xir/instructions/loop.h>
#include <runtime/xir/instructions/outline.h>
#include <runtime/xir/instructions/phi.h>
#include <runtime/xir/instructions/print.h>
#include <runtime/xir/instructions/raster_discard.h>
#include <runtime/xir/instructions/ray_query.h>
#include <runtime/xir/instructions/resource.h>
#include <runtime/xir/instructions/return.h>
#include <runtime/xir/instructions/store.h>
#include <runtime/xir/instructions/switch.h>
#include <runtime/xir/instructions/thread_group.h>
#include <runtime/xir/instructions/unreachable.h>
#include <runtime/xir/metadata.h>
#include <runtime/xir/metadata/comment.h>
#include <runtime/xir/metadata/location.h>
#include <runtime/xir/metadata/name.h>
#include <runtime/xir/module.h>
#include <runtime/xir/op.h>
#include <runtime/xir/passes/aggregate_field_bitmask.h>
#include <runtime/xir/passes/autodiff.h>
#include <runtime/xir/passes/call_graph.h>
#include <runtime/xir/passes/dce.h>
#include <runtime/xir/passes/dom_tree.h>
#include <runtime/xir/passes/early_return_elimination.h>
#include <runtime/xir/passes/local_load_elimination.h>
#include <runtime/xir/passes/local_store_forward.h>
#include <runtime/xir/passes/lower_ray_query_loop.h>
#include <runtime/xir/passes/mem2reg.h>
#include <runtime/xir/passes/outline.h>
#include <runtime/xir/passes/pointer_usage.h>
#include <runtime/xir/passes/promote_ref_arg.h>
#include <runtime/xir/passes/reg2mem.h>
#include <runtime/xir/passes/sroa.h>
#include <runtime/xir/passes/trace_gep.h>
#include <runtime/xir/passes/transpose_gep.h>
#include <runtime/xir/passes/unused_callable_removal.h>
#include <runtime/xir/pool.h>
#include <runtime/xir/special_register.h>
#include <runtime/xir/translators/ast2xir.h>
#include <runtime/xir/translators/json2xir.h>
#include <runtime/xir/translators/xir2json.h>
#include <runtime/xir/translators/xir2text.h>
#include <runtime/xir/undefined.h>
#include <runtime/xir/use.h>
#include <runtime/xir/user.h>
#include <runtime/xir/value.h>
#endif

