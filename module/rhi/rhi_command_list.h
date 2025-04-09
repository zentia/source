#pragma once
#include "context/rhi_context.h"
#include "pipeline/rhi_pipeline.h"
#include <array>

#include "taskflow/taskflow.hpp"

namespace source_module::rhi
{
	class rhi_command_list_base
	{
	public:
		void* alloc_command(int32_t alloc_size, int32_t alignment);
		rhi_pipeline_type switch_pipeline(rhi_pipeline_type type);
		[[nodiscard]] bool is_immediate() const;
		[[nodiscard]] bool is_executing() const;
		[[nodiscard]] bool is_bottom_of_pipe() const;
		[[nodiscard]] bool bypass() const;
		template <typename Lambda>
		void enqueue_lambda(Lambda&& lambda)
		{
			if (is_bottom_of_pipe())
			{
				lambda(*this);
			}
		}
	protected:
		struct persistent_state
		{
			bool m_immediate{ true };
		};
		persistent_state m_persistent_state_{};
		interface_command_context* m_graphics_context_{ nullptr };
		interface_compute_context* m_compute_context_{ nullptr };
		interface_rhi_upload_context* m_upload_context_{ nullptr };
		std::array<interface_compute_context*, 2> m_contexts_{};
		uint32_t m_num_commands_{ 0 };
		bool m_executing_{ false };
		bool m_allow_parallel_translate_{ true };
		rhi_pipeline_type m_pipeline_type_{ rhi_pipeline_type::none };
	private:
		void activate_pipelines(rhi_pipeline_type type);
	};

	class rhi_compute_command_list : public rhi_command_list_base
	{
	public:

	};

	class rhi_command_list : public rhi_compute_command_list
	{
	public:

	};
	class rhi_command_base
	{
	public:
		rhi_command_base* m_next;
		virtual void execute_and_destruct(rhi_command_list_base& command_list) = 0;
	};

	class rhi_lambda_command final : public rhi_command_base
	{
	public:

	};

	class rhi_command_list_immediate : public rhi_command_list
	{
	public:
		void immediate_flush();
		void initialize_immediate_contexts();
	};

	enum class rhi_submit_flags : uint8_t
	{
		none = 0,
		submit_gpu = 1 << 0,
		delete_resources = 1 << 1,
		flush_rhi_thread = 1 << 2,
		end_frame = 1 << 3,
	};

	class rhi_command_list_executor
	{
	public:
		static rhi_command_list_immediate& get_immediate_command_list();
		void latch_bypass();
		bool bypass() const;
		std::shared_ptr<tf::Taskflow> submit(std::span<rhi_command_list_base*> additional_command_lists, rhi_submit_flags submit_flags);
		rhi_command_list_immediate m_command_list_immediate;
	private:
		std::shared_ptr<tf::Taskflow> m_completion_event_{ nullptr };
		bool m_latched_bypass_{ false };
	};

	inline rhi_command_list_executor g_rhi_command_list_executor{};

}
