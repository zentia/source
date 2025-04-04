#pragma once
#include "Modules/rhi/rhi_module.h"

namespace source_module::rhi
{
	class direct3d12_rhi final : public source_module::rhi::rhi_module
	{
	public:
		void initialize(source_module::rhi::rhi_init_info init_info) override;
	};
}
