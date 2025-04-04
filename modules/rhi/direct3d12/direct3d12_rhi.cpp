#include "direct3d12_rhi.h"

namespace source_module::rhi
{
	void direct3d12_rhi::initialize(const rhi_init_info init_info)
	{
		m_window = init_info.window_module->get_window();
	}

}
