#pragma once

#include <luisa/core/spin_mutex.h>
#include <luisa/runtime/rtx/curve.h>

#include "fallback_prim.h"

namespace luisa::compute::fallback {

class FallbackCurve final : public FallbackPrim {

private:
    luisa::spin_mutex _geom_mutex;

public:
    [[nodiscard]] bool is_curve() const noexcept override { return true; }
    FallbackCurve(RTCDevice device, const AccelOption &option) noexcept;
    void build(luisa::unique_ptr<CurveBuildCommand> cmd) noexcept;
};

}// namespace luisa::compute::fallback
