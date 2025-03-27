#include "vulkan_rhi.h"

namespace source_runtime
{
	void vulkan_rhi::initialize(const rhi_init_info init_info)
	{
		m_window = init_info.window_system->GetWindow();

		const std::array<int, 2> window_size = init_info.window_system->GetWindowSize();
		const float width = static_cast<float>(window_size[0]);
		const float height = static_cast<float>(window_size[1]);
		m_viewport = { 0.0f,0.0f, width, height, 0.0f, 1.0f };
		m_scissor = {	{0,0},	{(uint32_t)(window_size[0]), (uint32_t)(window_size[1])}};
#ifdef DEBUG
		m_enable_validation_layers_ = true;
		m_enable_debug_utils_label_ = true;
#else
		m_enable_debug_utils_label_ = false;
		m_enable_debug_utils_label_ = false;
#endif
	}

	void vulkan_rhi::create_instance()
	{
		
	}

	bool vulkan_rhi::check_validation_layer_support()
	{
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);
		return true;
	}

}
