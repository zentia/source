#include <luisa/core/logging.h>
#include "fallback_prim.h"

namespace luisa::compute::fallback {

FallbackPrim::FallbackPrim(RTCDevice device, const AccelOption &option) noexcept
    : _handle{rtcNewScene(device)}, _geometry{nullptr}, _motion{option.motion} {
    luisa_fallback_accel_set_flags(_handle, option);
}

FallbackPrim::FallbackPrim(RTCDevice device, RTCGeometryType geom_type, const AccelOption &option) noexcept
    : FallbackPrim{device, option} { _create_geometry(geom_type); }

void FallbackPrim::_create_geometry(RTCGeometryType geom_type) noexcept {
    LUISA_DEBUG_ASSERT(_geometry == nullptr, "Geometry already created.");
    auto device = rtcGetSceneDevice(_handle);
    _geometry = rtcNewGeometry(device, geom_type);
    rtcSetGeometryMask(_geometry, ~0u);
    rtcAttachGeometry(_handle, _geometry);
    rtcReleaseGeometry(_geometry);// already moved into the scene
}

FallbackPrim::~FallbackPrim() noexcept {
    rtcReleaseScene(_handle);
}

}// namespace luisa::compute::fallback
