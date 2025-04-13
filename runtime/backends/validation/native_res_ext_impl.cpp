#include "native_res_ext_impl.h"
#include "buffer.h"
#include "texture.h"
#include "swap_chain.h"

namespace lc::validation {
using namespace luisa;
BufferCreationInfo NativeResourceExtImpl::register_external_buffer(
    void *buffer_ptr,
    const Type *element,
    size_t elem_count,
    // custom data see backends' header
    void *custom_data) noexcept {
    auto bf = _native->register_external_buffer(buffer_ptr, element, elem_count, custom_data);
    new Buffer{bf.handle, 0};
    return bf;
}

ResourceCreationInfo NativeResourceExtImpl::register_external_image(
    void *image_ptr,
    PixelFormat format, uint dimension,
    uint width, uint height, uint depth,
    uint mipmap_levels,
    // custom data see backends' header
    void *custom_data) noexcept {
    auto tex = _native->register_external_image(image_ptr, format, dimension, width, height, depth, mipmap_levels, custom_data);
    new Texture{tex.handle, dimension, false, uint3(0, 0, 0), format};
    return tex;
}

ResourceCreationInfo NativeResourceExtImpl::register_external_depth_buffer(
    void *depth_buffer_ptr,
    DepthFormat format,
    uint width,
    uint height,
    // custom data see backends' header
    void *custom_data) noexcept {
    auto tex = _native->register_external_depth_buffer(depth_buffer_ptr, format, width, height, custom_data);
    PixelFormat p_format;
    switch (format) {
        case DepthFormat::D16:
            p_format = PixelFormat::R16F;
            break;
        case DepthFormat::D24S8:
            p_format = PixelFormat::R32UInt;
            break;
        case DepthFormat::D32:
            p_format = PixelFormat::R32F;
            break;
        case DepthFormat::D32S8A24:
            p_format = PixelFormat::RG32UInt;
            break;
    }
    new Texture{tex.handle, 2, false, uint3(0, 0, 0), p_format};
    return tex;
}

SwapchainCreationInfo NativeResourceExtImpl::register_external_swapchain(
    void *swapchain_ptr,
    bool vsync) noexcept {
    auto chain = _native->register_external_swapchain(swapchain_ptr, vsync);
    new SwapChain(chain.handle);
    return chain;
}

uint64_t NativeResourceExtImpl::get_native_resource_device_address(
    void *native_handle) noexcept {
    return _native->get_native_resource_device_address(native_handle);
}
}// namespace lc::validation