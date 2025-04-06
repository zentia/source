#include "rhi_pipeline.h"

namespace source_module::rhi
{
	rhi_pipeline_type interface_command_context::get_pipeline() const
	{
		return rhi_pipeline_type::graphic;
	}

}
