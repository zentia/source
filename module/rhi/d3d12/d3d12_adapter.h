#pragma once

#include <vector>
#include <array>

#include "d3d12_device.h"
#include "module/rhi/multi_gpu.h"
#include <dxgi1_6.h>

namespace source_module::rhi
{
	class d3d12_device_basic_info
	{
	public:
		D3D_FEATURE_LEVEL max_feature_level;
		D3D_SHADER_MODEL max_shader_model;
		D3D12_RESOURCE_BINDING_TIER resource_binding_tier;
		D3D12_RESOURCE_HEAP_TIER resource_heap_tier;
		uint32_t num_device_nodes;
		bool supports_wave_ops;
		bool supports_atomic64;
		bool uma;

	};
	class d3d12_viewport;

	class d3d12_adapter_desc
	{
	public:
		d3d12_adapter_desc(const DXGI_ADAPTER_DESC& desc, int32_t adapter_index, const d3d12_device_basic_info& device_basic_info);
		static HRESULT enum_adapters(int adapter_index, DXGI_GPU_PREFERENCE gpu_preference, IDXGIFactory2* dxgi_factory2, IDXGIFactory6* dxgi_factory6, IDXGIAdapter** temp_adapter);
		DXGI_ADAPTER_DESC m_desc{};
		// Number of device nodes (read: GPUs)
		uint32_t m_num_device_nodes{ 1 };
	};
	class d3d12_adapter
	{
	public:
		explicit d3d12_adapter(const d3d12_adapter_desc& desc);
		void initialize();
		std::vector<d3d12_viewport*> m_viewports{};
		std::array<d3d12_device*, max_num_gpu> m_devices{};
		d3d12_adapter_desc m_desc;
		
	};
}
