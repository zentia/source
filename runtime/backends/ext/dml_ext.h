#pragma once

#include <luisa/runtime/rhi/device_interface.h>
#include <luisa/runtime/buffer.h>
#include <luisa/tensor/fused_activation.h>
namespace luisa::compute {

class DMLGraph {
public:
    virtual ~DMLGraph() noexcept = default;
    [[nodiscard]] virtual unique_ptr<Command> build() noexcept = 0;

protected:
    [[nodiscard]] virtual unique_ptr<Command> forward(
        Argument::Buffer input_buffer,
        Argument::Buffer output_buffer,
        Argument::Buffer weights_buffer) noexcept = 0;

public:
    [[nodiscard]] virtual size_t input_buffer_size_bytes() const noexcept = 0;
    [[nodiscard]] virtual size_t output_buffer_size_bytes() const noexcept = 0;
    [[nodiscard]] virtual size_t weight_buffer_size_bytes() const noexcept = 0;
    template<typename InputBuffer, typename OutputBuffer, typename WeightBuffer>
        requires is_buffer_or_view_v<InputBuffer> &&
                 is_buffer_or_view_v<OutputBuffer> &&
                 is_buffer_or_view_v<WeightBuffer>
    [[nodiscard]] unique_ptr<Command> forward(InputBuffer const &input, OutputBuffer const &output, WeightBuffer const &weights) noexcept {
        auto to_buffer_arg = []<typename T>(const T &t) noexcept {
            if constexpr (is_buffer_view_v<T>) {
                return Argument::Buffer{
                    t.handle(),
                    t.offset_bytes(),
                    t.size_bytes()};
            } else {
                return Argument::Buffer{
                    t.handle(),
                    0ull,
                    t.size_bytes()};
            }
        };
        return forward(to_buffer_arg(input), to_buffer_arg(output), to_buffer_arg(weights));
    }
};

class DirectMLExt : public DeviceExtension {
public:
    ~DirectMLExt() noexcept = default;
    static constexpr luisa::string_view name = "DirectMLExt";
    [[nodiscard]] virtual unique_ptr<DMLGraph> create_graph(
        uint32_t batch_size,
        uint32_t input_elements,
        uint32_t out_elements,
        luisa::span<const uint32_t> hidden_layer_elements,
        luisa::span<const FusedActivation> activations,
        bool half_precision = false) noexcept = 0;
};

}// namespace luisa::compute
