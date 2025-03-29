#pragma once

namespace source_runtime
{
	class world_module
	{
	public:
		void initialize();
		void tick(float delta_time);
	private:
		bool m_is_world_loaded_{ false };
	};
}
