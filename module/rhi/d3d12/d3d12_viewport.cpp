#include "d3d12_viewport.h"

#include <cassert>
#include <d3d12.h>
#include <dxgi.h>
#include <dxgi1_2.h>

#include "core_globals.h"
#include "d3d12_adapter.h"

namespace source_module::rhi
{
	d3d12_viewport::d3d12_viewport(d3d12_adapter* parent, HWND window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen) : d3d12_adapter_child(parent)
	{
		assert(source_runtime::core::is_in_game_thread());
		m_parent_adapter->m_viewports.push_back(this);
	}

	void d3d12_viewport::initialize()
	{
		const d3d12_adapter* adapter = m_parent_adapter;
		if (m_need_swap_chain)
		{
			std::shared_ptr<ID3D12CommandQueue> command_queue = adapter->m_devices[0]->m_queues[static_cast<uint32_t>(d3d12_queue_type::direct)].m_command_queue;
			uint32_t swap_chain_flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
			if (m_allow_tearing_)
			{
				swap_chain_flags |= DXGI_SWAP_CHAIN_FLAG_ALLOW_TEARING;
			}

			DXGI_SWAP_CHAIN_DESC1 swap_chain_desc1{};

			swap_chain_desc1.Width = m_size_x_;
			swap_chain_desc1.Height = m_size_y_;
		}
	}

}
