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
}
