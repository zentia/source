#include <DXApi/LCSwapChain.h>
#include <dxgi1_2.h>
#include <DXRuntime/Device.h>
#include <Resource/RenderTexture.h>
namespace lc::dx {
LCSwapChain::LCSwapChain(
    Device *device,
    CommandQueue *queue,
    GpuAllocator *resourceAllocator,
    HWND windowHandle,
    uint width,
    uint height,
    DXGI_FORMAT format,
    bool vsync,
    uint backBufferCount)
    : Resource(device), vsync(vsync) {
    this->format = format;
    frameCount = backBufferCount + 1;
    vstd::push_back_func(
        m_renderTargets,
        frameCount,
        [&] { return device; });
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
    swapChainDesc.BufferCount = frameCount;
    swapChainDesc.Width = width;
    swapChainDesc.Height = height;
    swapChainDesc.Format = format;
    swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
    if (!vsync)
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
    swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    swapChainDesc.SampleDesc.Count = 1;
    {
        IDXGISwapChain1 *localSwap;
        ThrowIfFailed(device->dxgiFactory->CreateSwapChainForHwnd(
            queue->Queue(),
            windowHandle,

            &swapChainDesc,
            nullptr,
            nullptr,
            &localSwap));

        swapChain = DxPtr(localSwap, true);
    }
    for (uint32_t n = 0; n < frameCount; n++) {
        ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n].rt)));
    }
    if (!vsync) {
        ComPtr<IDXGISwapChain3> swapChain3;
        auto hr = swapChain->QueryInterface(IID_PPV_ARGS(&swapChain3));
        if (hr == S_OK) {
            swapChain3->SetMaximumFrameLatency(backBufferCount * 2);
        } else {
            LUISA_WARNING("Can not get IDXGISwapChain3, please check your Direct-X runtime.");
        }
    }
}
LCSwapChain::LCSwapChain(
    PixelStorage &storage,
    Device *device,
    IDXGISwapChain1 *swapChain,
    bool vsync)
    : Resource(device),
      swapChain(swapChain, false),
      vsync(vsync) {
    DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChain->GetDesc1(&swapChainDesc);
    vstd::push_back_func(
        m_renderTargets,
        swapChainDesc.BufferCount,
        [&] { return device; });
    frameCount = swapChainDesc.BufferCount;
    for (uint32_t n = 0; n < swapChainDesc.BufferCount; n++) {
        ThrowIfFailed(swapChain->GetBuffer(n, IID_PPV_ARGS(&m_renderTargets[n].rt)));
    }
    storage = pixel_format_to_storage(TextureBase::ToPixelFormat(static_cast<GFXFormat>(swapChainDesc.Format)));
}
}// namespace lc::dx
