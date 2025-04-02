#include "render_module.h"

#include "Editor/base/Application/application.h"

namespace source_runtime
{
	void render_module::initialize(render_init_info& init_info)
	{
		std::shared_ptr<config_module> config_module = get_application().m_config_module;
		assert(config_module);

		// render context initialize
	}

	void render_module::tick(float delta_time)
	{
		
	}

	void render_module::process_swap_data()
	{
		
	}

}
