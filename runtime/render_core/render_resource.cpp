#include "render_resource.h"

#include "render_thread.h"
#include "module/rhi/rhi_command_list.h"


namespace source_runtime::render_core
{
	void render_resource::init_pre_rhi_resources()
	{
		source_module::rhi::rhi_command_list_immediate& rhi_cmd_list = source_module::rhi::rhi_command_list_executor::get_immediate_command_list();
	}

	void render_resource::init_resource(source_module::rhi::rhi_command_list_base& rhi_cmd_list)
	{
		if (m_list_index_ == -1)
		{
			m_list_index_ = render_resource_list::get(m_phase_).allocate(this);
		}
	}

	void begin_init_resource(render_resource* resource, render_command_pipe* render_command_pipe)
	{
		render_command_pipe::enqueue(render_command_pipe, [resource](source_module::rhi::rhi_command_list_base& rhi_cmd_list)
			{
				resource->init_resource(rhi_cmd_list);
			});
	}

}
