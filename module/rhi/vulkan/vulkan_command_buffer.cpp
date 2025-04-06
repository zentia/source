#include "vulkan_command_buffer.h"

#include <assert.h>

#include "runtime/utitlity/vector_extension.h"

namespace source_module::rhi
{

	void vulkan_command_buffer::alloc_memory()
	{
		assert(m_state == state::not_allocated);
		m_current_viewports.clear();
		m_current_scissors.clear();

		VkCommandBufferAllocateInfo command_buffer_allocate_info;
		command_buffer_allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		command_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		command_buffer_allocate_info.commandBufferCount = 1;
		//command_buffer_allocate_info.commandPool = 
	}

	vulkan_command_buffer* vulkan_command_buffer_pool::create()
	{
		if (!m_free_cmd_buffers_.empty())
		{
			vulkan_command_buffer* cmd_buffer = m_free_cmd_buffers_[0];
			source_runtime::vector::remove_at_swap(m_free_cmd_buffers_, 0);
		}
		return nullptr;
	}

}
