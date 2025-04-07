#include "rendering_thread.h"

#include <taskflow/taskflow.hpp>

namespace source_runtime::render_core
{
	render_command_pipe::render_command_pipe()
	{
		
	}

	void render_thread_command_pipe::enqueue_and_launch(std::unique_ptr<std::function<void(source_module::rhi::rhi_command_list_immediate&)>>&& function)
	{
		mutex_.lock();
		const bool empty = m_queues_[m_produce_index_].empty();
		m_queues_[m_produce_index_].emplace_back( std::move(function) );
		mutex_.unlock();

		if (empty)
		{
			tf::Taskflow task_flow;
			task_flow.emplace([this]()
				{

				});

			tf::Executor executor;
			executor.run(task_flow);
		}
	}

}
