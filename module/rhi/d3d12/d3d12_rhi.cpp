#include "d3d12_rhi.h"

#include <dxgi.h>

namespace source_module::rhi
{
	void d3d12_rhi::initialize(const rhi_init_info init_info)
	{
		m_window = init_info.window_module->get_window();
		find_adapter();
	}

	rhi_interface_type d3d12_rhi::get_interface_type() const
	{
		return rhi_interface_type::d3d12;
	}

	std::shared_ptr<rhi_viewport> d3d12_rhi::create_viewport(void* window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen, source_runtime::core::pixel_format pixel_format)
	{
		return nullptr;
	}

	d3d12_command_context* d3d12_rhi::create_command_context(d3d12_device* parent, d3d12_queue_type queue_type, bool is_default_context)
	{
		return new d3d12_command_context(parent, queue_type, is_default_context);
	}

	void d3d12_rhi::find_adapter()
	{
		IDXGIFactory6* dxgi_factory6 = nullptr;
		CreateDXGIFactory(__uuidof(IDXGIFactory6), reinterpret_cast<void**>(&dxgi_factory6));

		IDXGIAdapter* temp_adapter = nullptr;
		for (int i = 0; i < d3d12_adapter_desc::enum_adapters(i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, nullptr, dxgi_factory6, &temp_adapter) != DXGI_ERROR_NOT_FOUND; ++i)
		{
			if (temp_adapter)
			{
				d3d12_device_basic_info d3d12_device_basic_info{};
				DXGI_ADAPTER_DESC dxgi_adapter_desc{};
				assert(temp_adapter->GetDesc(&dxgi_adapter_desc) >= 0);
				d3d12_adapter_desc adapter_desc(dxgi_adapter_desc, i, d3d12_device_basic_info);
				
				m_chosen_adapters_.push_back(new d3d12_adapter(adapter_desc));
			}
		}
	}

	rhi_compute_context* d3d12_rhi::rhi_get_command_context(rhi_pipeline_type pipeline, rhi_gpu_mask gpu_mask)
	{
		d3d12_device* device = m_chosen_adapters_[0]->m_devices[gpu_mask.m_gpu_mask];

		d3d12_command_context* cmd_context;
		switch (pipeline)
		{
		case rhi_pipeline_type::graphic:
			//cmd_context = device->
			break;
		}
		return nullptr;
	}

}
