#include "rhi_command_list.h"

#include "rhi_global.h"
#include "runtime/application/application.h"
#include "runtime/memory/memory.h"
#include "runtime/render_core/render_thread.h"
#include "runtime/util/enum_range.h"

namespace source_module::rhi
{
	void* rhi_command_list_base::alloc_command(int32_t alloc_size, int32_t alignment)
	{
		rhi_command_base* result = static_cast<rhi_command_base*>(source_runtime::memory::allocator_allocate(alloc_size, alignment));
		++m_num_commands_;
		*m_command_link_ = result;
		m_command_link_ = &result->m_next;
		return result;
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

	bool rhi_command_list_base::allow_parallel_translate() const
	{
		if (!m_allow_parallel_translate_)
		{
			return false;
		}
		if (m_persistent_state_.m_immediate)
		{
			return false;
		}
		if (m_use_lock_fences_)
		{
			return false;
		}
		if (m_use_set_tracked_access_)
		{
			return false;
		}
		if (m_use_shader_bundles_ && !g_rhi_global.m_shader_bundles.m_supports_parallel)
		{
			return false;
		}
		return true;
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
		enqueue_lambda([
			new_pipeline = type,
			single_pipeline = is_single_rhi_pipeline(type)
		](rhi_command_list_base& executing_cmd_list)
		{
			executing_cmd_list.m_pipeline_type_ = new_pipeline;
			if (!single_pipeline)
			{
				executing_cmd_list.m_graphics_context_ = nullptr;
				executing_cmd_list.m_compute_context_ = nullptr;
			}

			for (rhi_pipeline_type pipeline : source_runtime::enum_range::make_flags_range(new_pipeline))
			{
				rhi_compute_context*& context = executing_cmd_list.m_contexts_[static_cast<uint8_t>(pipeline)];
				switch (pipeline)
				{
				case rhi_pipeline_type::graphic:
					{
						if (!context)
						{
							if (executing_cmd_list.allow_parallel_translate())
							{
								//context = source_runtime::application::instance()->m_rhi_module
							}
						}
					}
				}
			}
		});
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

}
