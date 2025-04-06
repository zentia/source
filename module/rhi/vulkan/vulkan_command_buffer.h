#pragma once
#include <vector>
#include <vulkan/vulkan_core.h>

namespace source_module::rhi
{
	class vulkan_command_buffer
	{
	public:
		enum class state : uint8_t
		{
			ready_for_begin,
			is_inside_begin,
			is_inside_render_pass,
			has_ended,
			submitted,
			not_allocated,
			need_reset
		};
		void alloc_memory();
		std::vector<VkViewport> m_current_viewports{};
		std::vector<VkRect2D> m_current_scissors{};
		state m_state{ state::not_allocated };
	protected:
		friend class vulkan_command_buffer_pool;
	};

	class vulkan_command_buffer_pool
	{
	public:
		
	private:
		vulkan_command_buffer* create();
		
		VkCommandPool m_pool_{ nullptr };
		std::vector<vulkan_command_buffer*> m_command_buffers_{};
		std::vector<vulkan_command_buffer*> m_free_cmd_buffers_{};
	};
}
