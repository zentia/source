#pragma once
#include "rhi_define.h"

namespace source_module::rhi
{
	class dynamic_rhi
	{
	public:
		virtual rhi_interface_type get_interface_type() const { return rhi_interface_type::hidden; }
	};
}
