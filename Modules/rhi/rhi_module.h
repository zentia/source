#pragma once

#define GLFW_INCLUDE_VULKAN
#include "Runtime/RenderCore/WindowSystem.h"

namespace source_runtime
{
	struct rhi_init_info
	{
		std::shared_ptr<Source::WindowSystem> window_system;
	};

	class rhi
	{
	public:
		virtual void initialize(rhi_init_info init_info) = 0;
	};
}
