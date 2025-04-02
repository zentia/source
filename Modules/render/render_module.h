#pragma once
#include <memory>

#include "Modules/window/window_module.h"

namespace source_runtime
{
	struct render_init_info
	{
		std::shared_ptr<window_module> window_system;
	};
	class render_module
	{
	public:
		void initialize(render_init_info& init_info);
		void tick(float delta_time);
	private:
		void process_swap_data();
	};
}
