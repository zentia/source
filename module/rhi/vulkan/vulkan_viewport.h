#pragma once
#include "runtime/core/pixel_format.h"
#include "vulkan_memory.h"
#include "module/rhi/rhi_resources.h"

namespace source_module::rhi
{
	class vulkan_viewport : public rhi_viewport, public device_child
	{
	public:
		enum : uint8_t
		{
			num_buffers = 3
		};
		vulkan_viewport(vulkan_device* device, void* window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen, source_runtime::core::pixel_format preferred_pixel_format);
	};
}
