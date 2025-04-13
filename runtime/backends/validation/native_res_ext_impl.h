#pragma once
#include <luisa/backends/ext/native_resource_ext_interface.h>
namespace lc::validation {
using namespace luisa::compute;
class NativeResourceExtImpl : public NativeResourceExt {
public:
    NativeResourceExt *_native;
    NativeResourceExtImpl(DeviceInterface *device, NativeResourceExt *native)
        : NativeResourceExt(device),
          _native(native) {}
    BufferCreationInfo register_external_buffer(
        void *buffer_ptr,
        const Type *element,
        size_t elem_count,
        // custom data see backends' header
        void *custom_data) noexcept override;

    ResourceCreationInfo register_external_image(
        void *image_ptr,
        PixelFormat format, uint dimension,
        uint width, uint height, uint depth,
        uint mipmap_levels,
        // custom data see backends' header
        void *custom_data) noexcept override;

    ResourceCreationInfo register_external_depth_buffer(
        void *depth_buffer_ptr,
        DepthFormat format,
        uint width,
        uint height,
        // custom data see backends' header
        void *custom_data) noexcept override;

    SwapchainCreationInfo register_external_swapchain(
        void *swapchain_ptr,
        bool vsync) noexcept override;

    uint64_t get_native_resource_device_address(
        void *native_handle) noexcept override;
};
}// namespace lc::validation