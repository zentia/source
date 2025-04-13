#include "dx_hdr_ext_impl.h"
#include "swap_chain.h"
namespace lc::validation {
SwapchainCreationInfo DXHDRExtImpl::create_swapchain(
    const DXSwapchainOption &option,
    uint64_t stream_handle) noexcept {
    auto sw = ptr->create_swapchain(option, stream_handle);
    new SwapChain(sw.handle);
    return sw;
}
auto DXHDRExtImpl::set_hdr_meta_data(
    uint64_t swapchain_handle,
    float max_output_nits,
    float min_output_nits,
    float max_cll,
    float max_fall,
    const DisplayChromaticities *custom_chroma) noexcept
    -> Meta {
    return ptr->set_hdr_meta_data(
        swapchain_handle,
        max_output_nits,
        min_output_nits,
        max_cll,
        max_fall,
        custom_chroma);
}
}// namespace lc::validation