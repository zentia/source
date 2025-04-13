#include <luisa/core/logging.h>

#include "fallback_buffer.h"
#include "fallback_mesh.h"

namespace luisa::compute::fallback {

FallbackMesh::FallbackMesh(RTCDevice device, const AccelOption &option) noexcept
    : FallbackPrim{device, RTC_GEOMETRY_TYPE_TRIANGLE, option} {}

void FallbackMesh::build(luisa::unique_ptr<MeshBuildCommand> cmd) noexcept {
    auto v_buffer = reinterpret_cast<FallbackBuffer *>(cmd->vertex_buffer())->data();
    auto t_buffer = reinterpret_cast<FallbackBuffer *>(cmd->triangle_buffer())->data();
    LUISA_DEBUG_ASSERT(cmd->vertex_buffer_size() % cmd->vertex_stride() == 0u, "Invalid vertex buffer size.");
    LUISA_DEBUG_ASSERT(cmd->triangle_buffer_size() % sizeof(Triangle) == 0u, "Invalid triangle buffer size.");
    auto v_count = cmd->vertex_buffer_size() / cmd->vertex_stride();
    auto t_count = cmd->triangle_buffer_size() / sizeof(Triangle);
    if (auto m = motion()) {
        LUISA_DEBUG_ASSERT(v_count % m.keyframe_count == 0u, "Invalid vertex buffer size.");
        auto v_count_per_keyframe = v_count / m.keyframe_count;
        auto v_buffer_offset = cmd->vertex_buffer_offset();
        auto v_stride = cmd->vertex_stride();
        auto v_buffer_stride = v_stride * v_count_per_keyframe;
        auto geom = geometry();
        rtcSetGeometryTimeRange(geom, m.time_start, m.time_end);
        rtcSetGeometryTimeStepCount(geom, m.keyframe_count);
        for (auto i = 0u; i < m.keyframe_count; i++) {
            rtcSetSharedGeometryBuffer(geom, RTC_BUFFER_TYPE_VERTEX, i, RTC_FORMAT_FLOAT3,
                                       v_buffer, v_buffer_offset + i * v_buffer_stride,
                                       v_stride, v_count_per_keyframe);
        }
    } else {
        rtcSetSharedGeometryBuffer(geometry(), RTC_BUFFER_TYPE_VERTEX, 0u, RTC_FORMAT_FLOAT3,
                                   v_buffer, cmd->vertex_buffer_offset(), cmd->vertex_stride(), v_count);
    }
    rtcSetSharedGeometryBuffer(geometry(), RTC_BUFFER_TYPE_INDEX, 0u, RTC_FORMAT_UINT3,
                               t_buffer, cmd->triangle_buffer_offset(), sizeof(Triangle), t_count);
    rtcCommitGeometry(geometry());
    rtcCommitScene(handle());
}

}// namespace luisa::compute::fallback
