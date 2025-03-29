#pragma once
#include "Modules/rhi/rhi_module.h"

namespace source_runtime
{
	struct render_init_info
	{
		std::shared_ptr<source_runtime::window_module> window_system;
	};
	class render_module
	{
	public:
		void initialize(render_init_info& init_info);
		void tick(float delta_time);
	private:
		void process_swap_data();
		std::shared_ptr<rhi> m_rhi_;
	};
}
