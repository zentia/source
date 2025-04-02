#include "Engine.h"

#include "Editor/base/Application/application.h"

namespace source_runtime
{
	bool gIsEditorMode{ false };
	const float source_engine::s_fps_alpha = 1.f / 100;

	void source_engine::start_engine(const std::string& configFilePath)
	{
		
	}

	void source_engine::clear()
	{
		get_application().clear();
	}

	void source_engine::shutdown_engine()
	{

	}

	float source_engine::calculate_delta_time()
	{
		float delta_time;
		{
			using namespace std::chrono;

			const steady_clock::time_point tick_time_pointer = steady_clock::now();
			const duration<float> time_span = duration_cast<duration<float>>(tick_time_pointer - m_last_tick_time_point_);
			delta_time = time_span.count();

			m_last_tick_time_point_ = tick_time_pointer;
		}
		return delta_time;
	}

	bool source_engine::tick_one_frame(const float delta_time)
	{
		logic_tick(delta_time);
		calculate_fps(delta_time);

		renderer_tick(delta_time);
		application& application = get_application();
		application.m_window_module->set_title(std::string("source - " + std::to_string(m_fps_) + "fps").c_str());
		const bool should_window_close = get_application().get_window_system()->should_close();
		return !should_window_close;
	}

	bool source_engine::renderer_tick(float delta_time)
	{
		return true;
	}

	void source_engine::calculate_fps(float delta_time)
	{
		m_frame_count_++;

		if (m_frame_count_ == 1)
		{
			m_average_duration_ = delta_time;
		}
		else
		{
			m_average_duration_ = m_average_duration_ * (1 - s_fps_alpha) + delta_time * s_fps_alpha;
		}

		m_fps_ = static_cast<int>(1.f / m_average_duration_);
	}

	void source_engine::logic_tick(float delta_time)
	{
		get_application().m_world_module->tick(delta_time);
		get_application().m_input_module->tick(delta_time);
	}

}
