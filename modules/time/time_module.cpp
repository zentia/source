#include "time_module.h"

#include <chrono>

namespace source_module::time
{
	void time_module::initialize()
	{
		
	}

	void time_module::update()
	{
		const std::chrono::steady_clock::time_point tick_time_pointer = std::chrono::steady_clock::now();
		const std::chrono::duration<float> time_span = std::chrono::duration_cast<std::chrono::duration<float>>(tick_time_pointer - m_last_tick_time_point_);
		m_delta_time = time_span.count();
		m_last_tick_time_point_ = tick_time_pointer;
	}

}
