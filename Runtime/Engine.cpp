#include "Engine.h"

#include "Editor/Src/Application/Application.h"
#include "RenderCore/WindowSystem.h"

namespace SourceRuntime
{
	bool gIsEditorMode{ false };

	void SourceEngine::StartEngine(const std::string& configFilePath)
	{
		
	}

	void SourceEngine::clear()
	{
		GetApplication().clear();
	}

	void SourceEngine::shutdown_engine()
	{

	}

	float SourceEngine::calculate_delta_time()
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

	bool SourceEngine::tick_one_frame(float delta_time)
	{
		const bool should_window_close = GetApplication().get_window_system()->ShouldClose();
		return !should_window_close;
	}

}
