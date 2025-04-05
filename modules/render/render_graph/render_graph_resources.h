#pragma once
#include "render_graph_parameter.h"
#include "modules/rhi/rhi_struct.h"

namespace source_module::render
{
	class rdg_resource
	{
	public:
		const char* name{ nullptr };
	protected:
		rhi::rhi_resource* m_rhi_resource_{ nullptr };
	};

	class rdg_uniform_buffer : public rdg_resource
	{
	public:

	private:
		const rdg_parameter m_rdg_parameter_;
	};
}
