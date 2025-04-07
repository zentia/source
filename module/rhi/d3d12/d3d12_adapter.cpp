#include "d3d12_adapter.h"

namespace source_module::rhi
{
	d3d12_adapter_desc::d3d12_adapter_desc(const DXGI_ADAPTER_DESC& desc, int32_t adapter_index, const d3d12_device_basic_info& device_basic_info)
	{
		
	}

	HRESULT d3d12_adapter_desc::enum_adapters(int adapter_index, DXGI_GPU_PREFERENCE gpu_preference, IDXGIFactory2* dxgi_factory2, IDXGIFactory6* dxgi_factory6, IDXGIAdapter** temp_adapter)
	{
		if (!dxgi_factory6 || gpu_preference == DXGI_GPU_PREFERENCE_UNSPECIFIED)
		{
			return dxgi_factory2->EnumAdapters(adapter_index, temp_adapter);
		}
		return dxgi_factory6->EnumAdapterByGpuPreference(adapter_index, gpu_preference, IID_PPV_ARGS(temp_adapter));
	}

	d3d12_adapter::d3d12_adapter(const d3d12_adapter_desc& desc) : m_desc(desc)
	{
		
	}


	void d3d12_adapter::initialize()
	{
		
	}

}
