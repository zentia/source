#pragma once

#include <luisa/runtime/rtx/mesh.h>
#include "fallback_prim.h"

namespace luisa::compute::fallback {

class FallbackMesh final : public FallbackPrim {

public:
    FallbackMesh(RTCDevice device, const AccelOption &option) noexcept;
    void build(luisa::unique_ptr<MeshBuildCommand> cmd) noexcept;
};

}// namespace luisa::compute::llvm
