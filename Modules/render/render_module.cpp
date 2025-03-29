#include "render_module.h"

#include "Editor/base/Application/application.h"
#include "Modules/rhi/rhi_module.h"
#include "Modules/rhi/vulkan/vulkan_rhi.h"

namespace source_runtime
{
	void render_module::initialize(render_init_info& init_info)
	{
		std::shared_ptr<config_module> config_module = get_application().m_config_module;
		Assert(config_module);

		// render context initialize
		rhi_init_info rhi_init_info;
		rhi_init_info.window_system = init_info.window_system;

		m_rhi_ = std::make_shared<vulkan_rhi>();
		m_rhi_->initialize(rhi_init_info);
	}

	void render_module::tick(float delta_time)
	{
		
	}

	void render_module::process_swap_data()
	{
		
	}

}
