#pragma once

#include "rhi_module.h"
#include "rhi_struct.h"

namespace source_runtime
{
	class vulkan_rhi final : public rhi
	{
	public:
		void initialize(const rhi_init_info init_info) override;
		void create_instance();
		GLFWwindow* m_window{ nullptr };
		rhi_viewport m_viewport;
		rhi_rect_2d m_scissor;
	private:
		bool check_validation_layer_support();
		bool m_enable_validation_layers_{ true };
		bool m_enable_debug_utils_label_{ true };
		bool m_enable_point_light_shadow_{ true };
	};
}
