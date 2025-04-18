#pragma once
#include <Resource/TextureBase.h>
#include <Resource/AllocHandle.h>
namespace lc::dx {
class RenderTexture final : public TextureBase {
private:
    AllocHandle allocHandle;
    mutable vstd::unordered_map<uint, uint> uavIdcs;
    mutable vstd::unordered_map<uint, uint> srvIdcs;
    mutable std::mutex allocMtx;

public:
    RenderTexture(
        Device *device,
        uint width,
        uint height,
        GFXFormat format,
        TextureDimension dimension,
        uint depth,
        uint mip,
        bool allowUav,
        bool allowSimul,
        bool allowRaster,
        GpuAllocator *allocator = nullptr,
        bool shared_adaptor = false);
    ~RenderTexture();
    D3D12_SHADER_RESOURCE_VIEW_DESC GetColorSrvDesc(uint mipOffset = 0) const override;
    ID3D12Resource *GetResource() const override {
        return allocHandle.resource.Get();
    }
    D3D12_RESOURCE_STATES GetInitState() const override {
        return D3D12_RESOURCE_STATE_COMMON;
    }
    D3D12_UNORDERED_ACCESS_VIEW_DESC GetColorUavDesc(uint targetMipLevel) const override;
    D3D12_RENDER_TARGET_VIEW_DESC GetRenderTargetDesc(uint mipOffset) const override;
    Tag GetTag() const override { return Tag::RenderTexture; }
    uint GetGlobalSRVIndex(uint mipOffset = 0) const override;
    uint GetGlobalUAVIndex(uint mipLevel) const override;
};
}// namespace lc::dx
