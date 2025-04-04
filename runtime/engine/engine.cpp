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
		application::instance()->clear();
	}

	void source_engine::shutdown_engine()
	{

	}

	bool source_engine::update()
	{
		application* application = application::instance();
		application->m_world_module->update();
		application->m_window_module->set_title(std::string("source - " + std::to_string(m_fps_) + "fps").c_str());
		const bool should_window_close = application::instance()->get_window_system()->should_close();
		return !should_window_close;
	}

	bool source_engine::late_update(float delta_time)
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


}
