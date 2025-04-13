#pragma once

#include <luisa/runtime/rtx/procedural_primitive.h>
#include "fallback_prim.h"

namespace luisa::compute::fallback {

class FallbackProceduralPrim final : public FallbackPrim {
public:
    FallbackProceduralPrim(RTCDevice device, const AccelOption &option) noexcept;
    void build(luisa::unique_ptr<ProceduralPrimitiveBuildCommand> cmd) noexcept;
};

}// namespace luisa::compute::fallback
