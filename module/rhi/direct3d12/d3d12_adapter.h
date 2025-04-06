#pragma once

#include <vector>
#include <array>

#include "d3d12_device.h"
#include "module/rhi/multi_gpu.h"

namespace source_module::rhi
{
	class d3d12_viewport;

	class d3d12_adapter_desc
	{
	public:
		
	};
	class d3d12_adapter
	{
	public:
		std::vector<d3d12_viewport*> m_viewports{};
		std::array<d3d12_device, max_num_gpu> m_devices{};
	};
}
