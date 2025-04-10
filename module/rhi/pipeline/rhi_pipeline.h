#pragma once
#include "glm/ext/vector_integer.hpp"

namespace source_module::rhi
{
	enum class rhi_pipeline_type : uint8_t
	{
		graphic = 1 << 0,
		async_compute = 1 << 1,

		none = 0,
		all = graphic | async_compute,
		num = 2,

	};

	constexpr bool is_single_rhi_pipeline(rhi_pipeline_type pipeline)
	{
		return pipeline != rhi_pipeline_type::none && glm::isPowerOfTwo(static_cast<uint8_t>(pipeline));
	}
	class rhi_compute_context
	{
	public:
		virtual rhi_pipeline_type get_pipeline() const
		{
			return rhi_pipeline_type::async_compute;
		}
	};

	class interface_command_context : public rhi_compute_context
	{
	public:
		rhi_pipeline_type get_pipeline() const override;
		virtual void rhi_draw_primitive(uint32_t base_vertex_index, uint32_t num_primitives, uint32_t num_instances) = 0;
	};
}
