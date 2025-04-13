#include <luisa/core/logging.h>

#include "fallback_device_api.h"
#include "fallback_buffer.h"
#include "fallback_proc_prim.h"

namespace luisa::compute::fallback {

FallbackProceduralPrim::FallbackProceduralPrim(RTCDevice device, const AccelOption &option) noexcept
    : FallbackPrim{device, RTC_GEOMETRY_TYPE_USER, option} {}

void FallbackProceduralPrim::build(luisa::unique_ptr<ProceduralPrimitiveBuildCommand> cmd) noexcept {
    auto aabb_buffer = reinterpret_cast<FallbackBuffer *>(cmd->aabb_buffer())->data();
    LUISA_DEBUG_ASSERT(cmd->aabb_buffer_size() % sizeof(AABB) == 0u, "Invalid AABB buffer size.");
    auto n = [&] {
        auto aabb_count = cmd->aabb_buffer_size() / sizeof(AABB);
        if (auto m = motion()) {
            LUISA_DEBUG_ASSERT(aabb_count % m.keyframe_count == 0u, "AABB count must be multiple of motion keyframe count.");
            auto aabb_count_per_keyframe = aabb_count / m.keyframe_count;
            auto geom = geometry();
            rtcSetGeometryUserPrimitiveCount(geom, aabb_count_per_keyframe);
            rtcSetGeometryTimeRange(geom, m.time_start, m.time_end);
            rtcSetGeometryTimeStepCount(geom, m.keyframe_count);
            return aabb_count_per_keyframe;
        }
        rtcSetGeometryUserPrimitiveCount(geometry(), aabb_count);
        return aabb_count;
    }();
    struct UserData {
        const void *ptr;
        size_t count;
    };
    UserData user_data{aabb_buffer, n};
    rtcSetGeometryUserData(geometry(), &user_data);
    rtcSetGeometryBoundsFunction(
        geometry(), [](const RTCBoundsFunctionArguments *args) noexcept {
            auto user_data = static_cast<const UserData *>(args->geometryUserPtr);
            auto aabb_buffer = static_cast<const AABB *>(user_data->ptr);
            auto aabb = aabb_buffer[args->timeStep * user_data->count + args->primID];
            *args->bounds_o = {
                .lower_x = aabb.packed_min[0],
                .lower_y = aabb.packed_min[1],
                .lower_z = aabb.packed_min[2],
                .align0 = 0.f,
                .upper_x = aabb.packed_max[0],
                .upper_y = aabb.packed_max[1],
                .upper_z = aabb.packed_max[2],
                .align1 = 0.f,
            };
        },
        nullptr);
    rtcSetGeometryIntersectFunction(geometry(), reinterpret_cast<RTCIntersectFunctionN>(api::luisa_fallback_ray_query_procedural_intersect_function));
    rtcSetGeometryOccludedFunction(geometry(), reinterpret_cast<RTCOccludedFunctionN>(api::luisa_fallback_ray_query_procedural_occluded_function));
    rtcCommitGeometry(geometry());
    rtcCommitScene(handle());
    rtcSetGeometryBoundsFunction(geometry(), nullptr, nullptr);
}

}// namespace luisa::compute::fallback
