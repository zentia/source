#include "d3d12_viewport.h"

namespace source_module::rhi
{
	d3d12_viewport::d3d12_viewport()
	{
		m_parent_adapter->m_viewports.push_back(this);
	}

}
