#pragma once
#include "module/rhi/dynamic_rhi.h"

namespace source_module::rhi
{
	class interface_d3d12_dynamic_rhi : public dynamic_rhi
	{
	public:
		rhi_interface_type get_interface_type() const override;
	};
}
