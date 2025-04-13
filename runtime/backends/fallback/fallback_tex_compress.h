#pragma once

#include <luisa/backends/ext/tex_compress_ext.h>

namespace luisa::compute::fallback {

class FallbackTexCompressInterface : public TexCompressExt {
public:
    virtual ~FallbackTexCompressInterface() noexcept = default;
};

[[nodiscard]] luisa::unique_ptr<FallbackTexCompressInterface>
create_fallback_tex_compress_ext() noexcept;

}// namespace luisa::compute::fallback
