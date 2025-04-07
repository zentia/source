#pragma once
#include "context/rhi_context.h"
#include "pipeline/rhi_pipeline.h"
#include <array>

namespace source_module::rhi
{
	class rhi_command_list_base
	{
	public:
	protected:
		interface_command_context* m_graphics_context_{ nullptr };
		interface_compute_context* m_compute_context_{ nullptr };
		interface_rhi_upload_context* m_upload_context_{ nullptr };
		std::array<interface_compute_context*, 2> m_contexts_{};
		uint32_t m_num_commands_{ 0 };
		bool m_executing_{ false };
		bool m_allow_parallel_translate_{ true };
	};

	class rhi_compute_command_list : public rhi_command_list_base
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
}
