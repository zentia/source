#include "rhi_command_list.h"

#include "runtime/render_core/render_thread.h"

namespace source_module::rhi
{
	rhi_pipeline_type rhi_command_list_base::switch_pipeline(rhi_pipeline_type type)
	{
		rhi_pipeline_type origin = m_pipeline_type_;

		return origin;
	}

	void rhi_command_list_base::activate_pipelines(rhi_pipeline_type type)
	{
		m_pipeline_type_ = type;

	}


	void rhi_command_list_immediate::immediate_flush()
	{
		
	}

	void rhi_command_list_immediate::initialize_immediate_contexts()
	{
		
	}

	rhi_command_list_immediate& rhi_command_list_executor::get_immediate_command_list()
	{
		return g_rhi_command_list_executor.m_command_list_immediate;
	}

	void rhi_command_list_executor::latch_bypass()
	{
		
	}

	std::shared_ptr<tf::Taskflow> rhi_command_list_executor::submit(std::span<rhi_command_list_base*> additional_command_lists, rhi_submit_flags submit_flags)
	{
		assert(source_runtime::core::is_in_rendering_thread());

		return m_completion_event_;
	}

}
