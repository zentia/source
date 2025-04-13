//
// Created by swfly on 2024/11/21.
//

#pragma once

#include <vector>

namespace luisa::compute::fallback {

struct alignas(16) FallbackBufferView {
    void *ptr;
    size_t size;
};

class FallbackBuffer {

private:
    size_t _size;
    std::byte *_data{};

public:
    explicit FallbackBuffer(size_t size);
    ~FallbackBuffer() noexcept;
    [[nodiscard]] auto data() noexcept { return _data; }
    [[nodiscard]] FallbackBufferView view(size_t offset, size_t size) noexcept;
    [[nodiscard]] FallbackBufferView view_with_offset(size_t offset) noexcept;
};

}// namespace luisa::compute::fallback
