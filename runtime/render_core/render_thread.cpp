#include "render_thread.h"


namespace source_runtime::render_core
{
	render_command_pipe::render_command_pipe()
	{
		
	}

	render_thread_command_pipe render_thread_command_pipe::instance_;

	void render_thread_command_pipe::enqueue_and_launch(command_list_function&& function)
	{
		mutex_.lock();
		const bool empty = m_queues_[m_produce_index_].empty();
		m_queues_[m_produce_index_].emplace_back(std::move(function));
		mutex_.unlock();

		if (empty)
		{
			
		}
	}

}
