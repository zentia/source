#pragma once
#include <luisa/backends/ext/dx_hdr_ext_interface.h>
#include <luisa/runtime/swapchain.h>
#include <luisa/runtime/stream.h>
namespace luisa::compute {
Swapchain DXHDRExt::create_swapchain(const Stream &stream, const DXSwapchainOption &option) noexcept {
    return Swapchain{stream.device(), create_swapchain(option, stream.handle())};
}
void DXHDRExt::set_color_space(
    Swapchain const &swapchain,
    ColorSpace const &color_space) const noexcept {
    set_color_space(swapchain.handle(), color_space);
}
auto DXHDRExt::set_hdr_meta_data(
    Swapchain const &swapchain,
    float max_output_nits,
    float min_output_nits,
    float max_cll,
    float max_fall,
    const DisplayChromaticities *custom_chroma) noexcept -> Meta {
    return set_hdr_meta_data(
        swapchain.handle(),
        max_output_nits,
        min_output_nits,
        max_cll,
        max_fall);
}
}// namespace luisa::compute