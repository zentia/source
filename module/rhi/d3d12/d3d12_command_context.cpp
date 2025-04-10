#include "d3d12_command_context.h"

#include "d3d12_queue.h"

namespace source_module::rhi
{
	d3d12_context_common::d3d12_context_common(d3d12_device* device, d3d12_queue_type queue_type, bool is_default_context)
		: m_device(device)
		, m_queue(queue_type)
		, m_is_default_context(is_default_context)
	{
		
	}
	d3d12_command_context::d3d12_command_context(d3d12_device* parent, d3d12_queue_type queue_type, bool is_default_context)
		: d3d12_context_common(parent, queue_type, is_default_context)
	{
		
	}

}
