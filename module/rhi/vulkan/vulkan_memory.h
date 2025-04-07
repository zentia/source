#pragma once
#include "vulkan_device.h"

namespace source_module::rhi
{
	class device_child
	{
	public:
		const vulkan_device* m_device;
	};
}
