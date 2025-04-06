#include "d3d12_viewport.h"

#include <cassert>
#include <d3d12.h>

#include "core_globals.h"
#include "d3d12_adapter.h"

namespace source_module::rhi
{
	d3d12_viewport::d3d12_viewport()
	{
		assert(source_runtime::core::is_in_game_thread());
		m_parent_adapter->m_viewports.push_back(this);
	}

	void d3d12_viewport::initialize()
	{
		if (m_need_swap_chain)
		{
			ID3D12CommandQueue* command_queue;
		}
	}

}
