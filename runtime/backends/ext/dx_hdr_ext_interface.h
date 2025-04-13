#pragma once
#include <luisa/runtime/rhi/device_interface.h>

namespace luisa::compute {
class Swapchain;
class Stream;
class DXHDRExt : public DeviceExtension {
public:
    enum class DisplayCurve {
        sRGB = 0,// The display expects an sRGB signal.
        ST2084,  // The display expects an HDR10 signal.
        None     // The display expects a linear signal.
    };
    enum class SwapChainBitDepth {
        _8 = 0,
        _10,
        _16,
    };
    struct DisplayChromaticities {
        float RedX{0.f};
        float RedY{0.f};
        float GreenX{0.f};
        float GreenY{0.f};
        float BlueX{0.f};
        float BlueY{0.f};
        float WhiteX{0.f};
        float WhiteY{0.f};
    };
    struct DXSwapchainOption {
        uint64_t window;
        uint2 size;
        PixelStorage storage = PixelStorage::HALF4;
        bool wants_vsync = true;
        uint back_buffer_count = 2;
    };

    enum class ColorSpace : uint {
        RGB_FULL_G22_NONE_P709 = 0,
        RGB_FULL_G10_NONE_P709 = 1,
        RGB_STUDIO_G22_NONE_P709 = 2,
        RGB_STUDIO_G22_NONE_P2020 = 3,
        RESERVED = 4,
        YCBCR_FULL_G22_NONE_P709_X601 = 5,
        YCBCR_STUDIO_G22_LEFT_P601 = 6,
        YCBCR_FULL_G22_LEFT_P601 = 7,
        YCBCR_STUDIO_G22_LEFT_P709 = 8,
        YCBCR_FULL_G22_LEFT_P709 = 9,
        YCBCR_STUDIO_G22_LEFT_P2020 = 10,
        YCBCR_FULL_G22_LEFT_P2020 = 11,
        RGB_FULL_G2084_NONE_P2020 = 12,
        YCBCR_STUDIO_G2084_LEFT_P2020 = 13,
        RGB_STUDIO_G2084_NONE_P2020 = 14,
        YCBCR_STUDIO_G22_TOPLEFT_P2020 = 15,
        YCBCR_STUDIO_G2084_TOPLEFT_P2020 = 16,
        RGB_FULL_G22_NONE_P2020 = 17,
        YCBCR_STUDIO_GHLG_TOPLEFT_P2020 = 18,
        YCBCR_FULL_GHLG_TOPLEFT_P2020 = 19,
        RGB_STUDIO_G24_NONE_P709 = 20,
        RGB_STUDIO_G24_NONE_P2020 = 21,
        YCBCR_STUDIO_G24_LEFT_P709 = 22,
        YCBCR_STUDIO_G24_LEFT_P2020 = 23,
        YCBCR_STUDIO_G24_TOPLEFT_P2020 = 24,
        CUSTOM = 0xFFFFFFFF
    };

    struct Meta {
        DisplayChromaticities chromaticities;
    };

    struct DisplayData {
        uint bits_per_color;
        ColorSpace color_space;
        float2 red_primary;
        float2 green_primary;
        float2 blue_primary;
        float2 white_point;
        float min_luminance;
        float max_luminance;
        float max_full_frame_luminance;
    };

    [[nodiscard]] virtual SwapchainCreationInfo create_swapchain(
        const DXSwapchainOption &option,
        uint64_t stream_handle) noexcept = 0;

    virtual Meta set_hdr_meta_data(
        uint64_t swapchain_handle,
        float max_output_nits = 1000.0f,
        float min_output_nits = 0.001f,
        float max_cll = 2000.0f,
        float max_fall = 500.0f,
        const DisplayChromaticities *custom_chroma = nullptr) noexcept = 0;

    static constexpr luisa::string_view name = "DXHDRExt";
    [[nodiscard]] Swapchain create_swapchain(const Stream &stream, const DXSwapchainOption &option) noexcept;
    [[nodiscard]] virtual bool device_support_hdr() const noexcept = 0;
    virtual void set_color_space(
        uint64_t handle,
        ColorSpace const &color_space) const noexcept = 0;
    void set_color_space(
        Swapchain const &swapchain,
        ColorSpace const &color_space) const noexcept;
    Meta set_hdr_meta_data(
        Swapchain const &swapchain,
        float max_output_nits = 1000.0f,
        float min_output_nits = 0.001f,
        float max_cll = 2000.0f,
        float max_fall = 500.0f,
        const DisplayChromaticities *custom_chroma = nullptr) noexcept;
    [[nodiscard]] virtual DisplayData get_display_data(uint64_t hwnd) const noexcept = 0;
protected:
    ~DXHDRExt() = default;
};
}// namespace luisa::compute