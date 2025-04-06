#include "interface_vulkan_dynamic_rhi.h"

namespace source_module::rhi
{
	rhi_interface_type interface_vulkan_dynamic_rhi::get_interface_type() const
	{
		return rhi_interface_type::vulkan;
	}

}
