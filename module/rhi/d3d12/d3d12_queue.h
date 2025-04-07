#pragma once
#include "d3d12_submission.h"

namespace source_module::rhi
{
	class d3d12_device;

	enum class d3d12_queue_type : uint8_t
	{
		direct = 0,
		copy = 1,
		async = 2,
		count = 3,
	};


}
