#include "render_module.h"

#include "Editor/Src/Application/Application.h"
#include "Modules/Config/ConfigModule.h"
#include "Modules/rhi/rhi_module.h"

namespace source_runtime
{
	void render_module::initialize(render_init_info& init_info)
	{
		std::shared_ptr<config_module> config_module = GetApplication().m_config_module;
		Assert(config_module);

		// render context initialize
		rhi_init_info rhi_init_info;
		rhi_init_info.window_system = init_info.window_system;


	}

	void render_module::tick(float delta_time)
	{
		
	}

	void render_module::process_swap_data()
	{
		
	}

}
