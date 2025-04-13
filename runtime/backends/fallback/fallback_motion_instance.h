#pragma once

#include <luisa/core/spin_mutex.h>
#include <luisa/runtime/rhi/resource.h>
#include <luisa/runtime/rhi/command.h>

#include "fallback_prim.h"

namespace luisa::compute::fallback {

class FallbackMotionInstance final : public FallbackPrimBase {

private:
    luisa::spin_mutex _build_mutex;
    AccelMotionOption _option;
    FallbackPrim *_child;
    luisa::vector<MotionInstanceTransform> _transforms;

public:
    explicit FallbackMotionInstance(const AccelMotionOption &option) noexcept;
    void build(luisa::unique_ptr<MotionInstanceBuildCommand> cmd) noexcept;
    [[nodiscard]] bool is_motion_instance() const noexcept override { return true; }
    [[nodiscard]] auto option() const noexcept { return _option; }
    [[nodiscard]] auto child() const noexcept { return _child; }
    [[nodiscard]] auto transforms() const noexcept { return luisa::span{_transforms}; }
};

}// namespace luisa::compute::fallback
