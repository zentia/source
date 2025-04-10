#include "render_thread.h"

#include <taskflow.hpp>

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
			tf::Taskflow task_flow;
			task_flow.emplace([this]()
			{
				source_module::rhi::rhi_command_list_immediate& rhi_command_list = source_module::rhi::rhi_command_list_executor::get_immediate_command_list();

				mutex_.lock();
				std::vector<command>& consume_commands = m_queues_[m_produce_index_];
				m_produce_index_ ^= 1;
				mutex_.unlock();

				for (command& consume_command : consume_commands)
				{
					(consume_command.m_function)(rhi_command_list);
					consume_command.m_function = nullptr;
				}
				consume_commands.clear();
			});

			tf::Executor executor;
			executor.run(task_flow);
		}
	}

}
