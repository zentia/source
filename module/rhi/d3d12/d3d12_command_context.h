#pragma once
#include "d3d12_device.h"

namespace source_module::rhi
{
	class d3d12_command_context_base
	{
	public:
		
	};
	class d3d12_context_common
	{
	public:
		
	};

	class d3d12_command_context : public d3d12_context_common
	{
	public:
		d3d12_command_context(d3d12_device* parent, d3d12_queue_type queue_type, bool is_default_context);
	};
}
