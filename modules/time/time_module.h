#pragma once
#include <chrono>

namespace source_module::time
{
	class time_module
	{
	public:
		void initialize();
		void update();
		float m_delta_time{ 0 };
	private:
		std::chrono::steady_clock::time_point m_last_tick_time_point_{ std::chrono::steady_clock::now() };
		std::chrono::steady_clock::time_point m_world_start_time_point_{ std::chrono::steady_clock::now() };
	};
}
