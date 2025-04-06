#include "interface_d3d12_dynamic_rhi.h"

namespace source_module::rhi
{
	rhi_interface_type interface_d3d12_dynamic_rhi::get_interface_type() const
	{
		return rhi_interface_type::d3d12;
	}

}
