//
// Created by Mike Smith on 2022/2/11.
//

#pragma once

#include <luisa/runtime/rhi/sampler.h>
#include <luisa/runtime/rhi/command.h>
#include "../common/resource_tracker.h"
#include "fallback_texture.h"

namespace luisa {
class ThreadPool;
}// namespace luisa

namespace luisa::compute::fallback {

class FallbackBindlessArray {

public:
    struct alignas(16) Slot {
        const void *buffer{nullptr};
        uint64_t _compressed_buffer_size_sampler_2d_sampler_3d{};// [63:8] compressed buffer offset, [7:4] sampler 2d, [3:0] sampler 3d
        const FallbackTexture *tex2d{nullptr};
        const FallbackTexture *tex3d{nullptr};

        [[nodiscard]] auto buffer_size() const noexcept { return _compressed_buffer_size_sampler_2d_sampler_3d >> 8u; }
        [[nodiscard]] auto sampler_2d() const noexcept { return Sampler::decode(_compressed_buffer_size_sampler_2d_sampler_3d >> 4u & 0xfu); }
        [[nodiscard]] auto sampler_3d() const noexcept { return Sampler::decode(_compressed_buffer_size_sampler_2d_sampler_3d & 0xfu); }
        void set_buffer_size(size_t offset) noexcept {
            _compressed_buffer_size_sampler_2d_sampler_3d = (offset << 8u) | (_compressed_buffer_size_sampler_2d_sampler_3d & ~0xffull);
        }
        void set_sampler_2d(uint code) noexcept {
            _compressed_buffer_size_sampler_2d_sampler_3d = (code << 4u) | (_compressed_buffer_size_sampler_2d_sampler_3d & ~0xf0ull);
        }
        void set_sampler_3d(uint code) noexcept {
            _compressed_buffer_size_sampler_2d_sampler_3d = (code) | (_compressed_buffer_size_sampler_2d_sampler_3d & ~0x0full);
        }
        void set_sampler_2d(Sampler sampler) noexcept { set_sampler_2d(sampler.code()); }
        void set_sampler_3d(Sampler sampler) noexcept { set_sampler_3d(sampler.code()); }
    };

    struct alignas(16) View {
        Slot *slots{nullptr};
        size_t size{0};
    };

private:
    luisa::vector<Slot> _slots{};

public:
    explicit FallbackBindlessArray(size_t capacity) noexcept;
    [[nodiscard]] auto &slot(unsigned int idx) const noexcept { return _slots[idx]; }
    void update(luisa::unique_ptr<BindlessArrayUpdateCommand> cmd) noexcept;
    [[nodiscard]] auto view() noexcept { return View{_slots.data(), _slots.size()}; }
    [[nodiscard]] auto native_handle() noexcept { return _slots.data(); }
    [[nodiscard]] auto native_handle() const noexcept { return _slots.data(); }
};

using FallbackBindlessArrayView = FallbackBindlessArray::View;

}// namespace luisa::compute::fallback
