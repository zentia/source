#pragma once

#include <luisa/vstl/meta_lib.h>
#include <luisa/backends/ext/dx_hdr_ext_interface.h>

namespace lc::validation {

using namespace luisa::compute;

class DXHDRExtImpl : public DXHDRExt, public vstd::IOperatorNewBase {
public:
    DXHDRExt *ptr;
    DXHDRExtImpl(DXHDRExt *ptr) noexcept : ptr(ptr) {}
    SwapchainCreationInfo create_swapchain(
        const DXSwapchainOption &option,
        uint64_t stream_handle) noexcept override;
    Meta set_hdr_meta_data(
        uint64_t swapchain_handle,
        float max_output_nits = 1000.0f,
        float min_output_nits = 0.001f,
        float max_cll = 2000.0f,
        float max_fall = 500.0f,
        const DisplayChromaticities *custom_chroma = nullptr) noexcept override;
    bool device_support_hdr() const noexcept override {
        return ptr->device_support_hdr();
    }
    void set_color_space(
        uint64_t handle,
        ColorSpace const &color_space) const noexcept override {
        ptr->set_color_space(handle, color_space);
    }
    DisplayData get_display_data(uint64_t hwnd) const noexcept override {
        return ptr->get_display_data(hwnd);
    }
};

}// namespace lc::validation
