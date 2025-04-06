#pragma once

namespace source_module::rhi
{
	class d3d12_adapter;

	class d3d12_adapter_child
	{
	public:
		d3d12_adapter* m_parent_adapter{ nullptr };
	};

	class d3d12_gpu_object
	{
	public:
		
	};

	class d3d12_single_node_gpu_object : public d3d12_gpu_object
	{
	public:
		
	};
}
