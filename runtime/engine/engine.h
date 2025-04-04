#pragma once
#include <chrono>
#include <string>

namespace source_runtime
{
	class source_engine
	{
	public:
		void start_engine(const std::string& configFilePath);
		void clear();
		void shutdown_engine();
		float calculate_delta_time();
		bool update();
		bool late_update(float delta_time);
		void calculate_fps(float delta_time);
	protected:
		static const float s_fps_alpha;
		float m_average_duration_{ 0.0f };
		int m_frame_count_{ 0 };
		int m_fps_{ 0 };
	};
}
