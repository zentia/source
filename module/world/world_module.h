#pragma once

namespace source_runtime
{
	class world_module
	{
	public:
		void initialize();
		void update();
	private:
		bool m_is_world_loaded_{ false };
	};
}
