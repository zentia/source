#pragma once
#include <string>

namespace SourceRuntime
{
	class SourceEngine
	{
	public:
		void StartEngine(const std::string& configFilePath);
		void clear();
		void shutdown_engine();
		float calculate_delta_time();
		bool tick_one_frame(float delta_time);
	protected:
		std::chrono::steady_clock::time_point m_last_tick_time_point_{ std::chrono::steady_clock::now() };
	};
}
