#pragma once
#include <DXApi/LCCmdBuffer.h>
#include <Resource/RenderTexture.h>
#include <DXApi/LCDevice.h>
#include <Resource/SwapChain.h>
#include <DXRuntime/CommandQueue.h>
#include <DXRuntime/DxPtr.h>
namespace lc::dx {
class LCSwapChain : public Resource {
public:
    vstd::vector<SwapChain> m_renderTargets;
    DxPtr<IDXGISwapChain1> swapChain;
    uint64 frameIndex = 0;
    uint64 frameCount = 0;
    DXGI_FORMAT format;
    bool vsync;
    Tag GetTag() const override { return Tag::SwapChain; }
    LCSwapChain(
        Device *device,
        CommandQueue *queue,
        GpuAllocator *resourceAllocator,
        HWND windowHandle,
        uint width,
        uint height,
        DXGI_FORMAT format,
        bool vsync,
        uint backBufferCount);
    LCSwapChain(
        PixelStorage& storage,
        Device* device,
        IDXGISwapChain1* swapChain,
        bool vsync);
};
}// namespace lc::dx
