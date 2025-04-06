#pragma once
#include "d3d12_adapter.h"

namespace source_module::rhi
{
	class d3d12_adapter_child
	{
	public:
		d3d12_adapter* m_parent_adapter{ nullptr };
	};
}
