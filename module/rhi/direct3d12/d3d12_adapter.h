#pragma once
#include <vector>

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
	};
}
