//
// Created by swfly on 2024/12/2.
//

#pragma once

#include <luisa/runtime/swapchain.h>

namespace luisa::compute::fallback {

class FallbackTexture;
class FallbackStream;

class FallbackSwapchain {
private:
    FallbackStream *_bound_stream;
    void *_handle;
    luisa::uint2 size;

public:
    FallbackSwapchain(FallbackStream *bound_stream,
                      const SwapchainOption &option) noexcept;
    ~FallbackSwapchain() noexcept;
    void present(FallbackStream *stream, FallbackTexture *frame);
};

}// namespace luisa::compute::fallback