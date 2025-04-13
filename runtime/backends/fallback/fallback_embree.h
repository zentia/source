#pragma once

#if LUISA_COMPUTE_FALLBACK_EMBREE_VERSION == 3
#include <embree3/rtcore.h>
#else
#include <embree4/rtcore.h>
#endif

#include <luisa/runtime/rhi/resource.h>

namespace luisa::compute::fallback {

inline void luisa_fallback_accel_set_flags(RTCScene scene, const AccelOption &option) noexcept {
    auto scene_flags = 0u;
#if LUISA_COMPUTE_FALLBACK_EMBREE_VERSION == 3
    scene_flags |= RTC_SCENE_FLAG_CONTEXT_FILTER_FUNCTION;
#endif
    if (option.allow_compaction) { scene_flags |= RTC_SCENE_FLAG_COMPACT; }
    if (option.allow_update) { scene_flags |= RTC_SCENE_FLAG_DYNAMIC; }
    rtcSetSceneFlags(scene, static_cast<RTCSceneFlags>(scene_flags));
    switch (option.hint) {
        case AccelOption::UsageHint::FAST_TRACE:
            rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_HIGH);
            break;
        case AccelOption::UsageHint::FAST_BUILD:
            rtcSetSceneBuildQuality(scene, RTC_BUILD_QUALITY_MEDIUM);
            break;
    }
}

}// namespace luisa::compute::fallback
