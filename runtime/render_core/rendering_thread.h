#pragma once
#include <functional>
#include <memory>
#include <mutex>
#include <array>

#include "core_globals.h"
#include "module/rhi/rhi_command_list.h"

namespace source_runtime::render_core
{
	inline bool g_is_threaded_rendering{ false };
	template<typename Str>
	struct render_command_tag
	{

	};
	
	class render_command_pipe
	{
	public:
		using command_list_function = std::unique_ptr<std::function<void(source_module::rhi::rhi_command_list&)>>;
		using empty_function = std::unique_ptr<std::function<void()>>;

		render_command_pipe();

		static void enqueue(render_command_pipe* pipe, command_list_function&& function)
		{

		}
	};

	class render_thread_command_pipe
	{
	public:
		template <typename RenderCommandTag, typename LambdaType>
		static void enqueue(LambdaType&& lambda)
		{
			
		}
	private:
		static render_thread_command_pipe instance;

		void enqueue_and_launch(std::unique_ptr<std::function<void(source_module::rhi::rhi_command_list_immediate&)>>&& function);

		struct command
		{
			explicit command(std::unique_ptr<std::function<void(source_module::rhi::rhi_command_list_immediate&)>>&& function) : m_function(std::move(function))
			{}
			std::unique_ptr<std::function<void(source_module::rhi::rhi_command_list_immediate&)>> m_function;
		};
		int32_t m_produce_index_ = 0;
		std::array<std::vector<command>, 2> m_queues_;
		std::mutex mutex_;
	};
	template<typename RenderCommandTag, typename LambdaType>
	void enqueue_unique_render_command(LambdaType&& lambda)
	{
		if (core::is_in_rendering_thread())
		{
			lambda(source_module::rhi::rhi_command_list_executor::get_immediate_command_list());
		}
		else if (g_is_threaded_rendering || !core::is_in_game_thread())
		{
			
		}
	}
}
