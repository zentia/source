#pragma once
#include "module/rhi/pipeline/rhi_pipeline.h"

namespace source_module::rhi
{
	class vulkan_context_common
	{
	public:
		
	};

	class vulkan_command_list_context : public vulkan_context_common, public interface_command_context
	{
	public:
		void rhi_draw_primitive(uint32_t base_vertex_index, uint32_t num_primitives, uint32_t num_instances) override;
	};
}
