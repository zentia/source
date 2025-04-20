#include "ui_module.h"

#include "runtime/application/application.h"

namespace source_runtime::ui
{
	void ui_module::initialize(window_ui_init_info init_info)
	{
		m_is_vulkan_ = m_rhi_->get_interface_type() == source_module::rhi::rhi_interface_type::vulkan;
	}

	void ui_module::update()
	{
		if (m_canvas_)
		{
			m_canvas_->draw();
		}
	}

}
