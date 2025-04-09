#include "rhi_command_list.h"

#include "runtime/render_core/render_thread.h"

namespace source_module::rhi
{
	void* rhi_command_list_base::alloc_command(int32_t alloc_size, int32_t alignment)
	{
		//rhi_command_base* result = new rhi_command_base();
		return nullptr;
	}

	rhi_pipeline_type rhi_command_list_base::switch_pipeline(rhi_pipeline_type type)
	{
		rhi_pipeline_type origin = m_pipeline_type_;
		activate_pipelines(type);
		return origin;
	}

	bool rhi_command_list_base::is_immediate() const
	{
		return m_persistent_state_.m_immediate;
	}

	bool rhi_command_list_base::is_executing() const
	{
		return m_executing_;
	}

	bool rhi_command_list_base::is_bottom_of_pipe() const
	{
		return bypass() || is_executing();
	}

	bool rhi_command_list_base::bypass() const
	{
		return g_rhi_command_list_executor.bypass() && is_immediate();
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

	bool rhi_command_list_executor::bypass() const
	{
		return m_latched_bypass_;
	}

	std::shared_ptr<tf::Taskflow> rhi_command_list_executor::submit(std::span<rhi_command_list_base*> additional_command_lists, rhi_submit_flags submit_flags)
	{
		assert(source_runtime::core::is_in_rendering_thread());

		return m_completion_event_;
	}

}
