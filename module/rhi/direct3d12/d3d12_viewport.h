#pragma once
#include "d3d12_rhi_common.h"
#include "windows_critical_section.h"
#include "module/rhi/rhi_resources.h"

namespace source_module::rhi
{
	class d3d12_viewport : public rhi_viewport, public d3d12_adapter_child
	{
	public:
		d3d12_viewport();
		static critical_section m_dxgi_back_buffer_lock;
	};
}
