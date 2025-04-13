//
// Created by Mike Smith on 2022/2/7.
//

#pragma once

#include "fallback_command_queue.h"

namespace luisa::compute::fallback {

class FallbackStream final {

private:
    FallbackCommandQueue _queue;

#define LUISA_FALLBACK_STREAM_ENQUEUE_COMMAND_DECL(COMMAND_TYPE) \
    void _enqueue(luisa::unique_ptr<COMMAND_TYPE> cmd) noexcept;
    LUISA_MAP(LUISA_FALLBACK_STREAM_ENQUEUE_COMMAND_DECL, LUISA_COMPUTE_RUNTIME_COMMANDS)
#undef LUISA_FALLBACK_STREAM_ENQUEUE_COMMAND_DECL

public:
    explicit FallbackStream(size_t queue_size = 8u) noexcept;
    ~FallbackStream() noexcept = default;
    [[nodiscard]] auto queue() noexcept { return &_queue; }
    [[nodiscard]] auto native_handle() noexcept { return queue(); }
    void synchronize() noexcept { _queue.synchronize(); }
    void dispatch(CommandList &&cmd_list) noexcept;
    void dispatch(luisa::move_only_function<void()> &&f) noexcept;
};

}// namespace luisa::compute::fallback
