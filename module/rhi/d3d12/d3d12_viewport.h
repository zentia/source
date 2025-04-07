#pragma once
#include "d3d12_rhi_common.h"
#include "windows_critical_section.h"
#include "module/rhi/rhi_resources.h"

namespace source_module::rhi
{
	class d3d12_viewport : public rhi_viewport, public d3d12_adapter_child
	{
	public:
		d3d12_viewport(d3d12_adapter* parent, HWND window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen);
		void initialize();
		static critical_section m_dxgi_back_buffer_lock;
		bool m_need_swap_chain{ false };
	private:
		uint32_t m_size_x_{ 0 };
		uint32_t m_size_y_{ 0 };
		bool m_allow_tearing_{ true };
	};
}
