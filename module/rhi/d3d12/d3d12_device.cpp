#include "d3d12_device.h"

namespace source_module::rhi
{
	d3d12_device::d3d12_device(d3d12_adapter* adapter) : d3d12_adapter_child(adapter)
	{
		
	}

	d3d12_context_common* d3d12_device::obtain_context(d3d12_queue_type queue_type)
	{
		//d3d12_context_common* context = m_queues[static_cast<uint32_t>(queue_type)];
		return nullptr;
	}

}
