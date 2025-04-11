#pragma once

#include <utility>

#include <runtime/core/spin_mutex.h>
#include <runtime/runtime/rhi/resource.h>
#include <runtime/runtime/event.h>
#include <runtime/runtime/command_list.h>
#include <runtime/runtime/image.h>
#include <runtime/runtime/stream_event.h>

namespace luisa::compute {

class LC_RUNTIME_API Swapchain final : public Resource {

public:
    struct LC_RUNTIME_API Present {
        const Swapchain *chain{nullptr};
        ImageView<float> frame;
        void operator()(DeviceInterface *device, uint64_t stream_handle) && noexcept;
    };

private:
    friend class Device;
    friend class ResourceGenerator;
    friend class DXHDRExt;
    PixelStorage _storage{};
    Swapchain(DeviceInterface *device, const SwapchainCreationInfo &create_info) noexcept;
    Swapchain(DeviceInterface *device, const SwapchainOption &option, uint64_t stream_handle) noexcept;

public:
    Swapchain() noexcept = default;
    ~Swapchain() noexcept override;
    using Resource::operator bool;
    Swapchain(Swapchain &&) noexcept = default;
    Swapchain(Swapchain const &) noexcept = delete;
    Swapchain &operator=(Swapchain &&rhs) noexcept {
        _move_from(std::move(rhs));
        return *this;
    }
    Swapchain &operator=(Swapchain const &) noexcept = delete;
    [[nodiscard]] PixelStorage backend_storage() const {
        _check_is_valid();
        return _storage;
    }
    [[nodiscard]] Present present(ImageView<float> frame) const noexcept;
};

LUISA_MARK_STREAM_EVENT_TYPE(Swapchain::Present)

}// namespace luisa::compute
