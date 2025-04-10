#pragma once

namespace source_module::rhi
{
	enum : std::uint8_t
	{
		max_num_gpu = 1,
	};

	struct rhi_gpu_mask
	{
		uint32_t m_gpu_mask{0};
	};
}
