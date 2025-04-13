#include <luisa/core/logging.h>
#include "fallback_motion_instance.h"

namespace luisa::compute::fallback {

FallbackMotionInstance::FallbackMotionInstance(const AccelMotionOption &option) noexcept
    : _option{option}, _child{nullptr} {
    LUISA_ASSERT(option.keyframe_count <= 255u,
                 "Motion keyframe count must be less than 256 (got {}).",
                 option.keyframe_count);
}

void FallbackMotionInstance::build(luisa::unique_ptr<MotionInstanceBuildCommand> cmd) noexcept {
    LUISA_DEBUG_ASSERT(cmd->keyframes().size() == _option.keyframe_count,
                       "Motion keyframe count mismatch {} vs {}.",
                       cmd->keyframes().size(), _option.keyframe_count);
    std::scoped_lock lock{_build_mutex};
    _child = reinterpret_cast<FallbackPrim *>(cmd->child());
    _transforms = cmd->steal_keyframes();
}

}// namespace luisa::compute::fallback
