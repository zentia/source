#pragma once
#include <vulkan/vulkan_core.h>

#include "module/rhi/rhi_resources.h"


#define VULKAN_RHI_RESOURCE_DECLARE(PARENT, SELF, TYPE) \
	class SELF : public PARENT \
	{ \
	public: \
		void set_resource(const TYPE res) \
		{ \
			m_resource_ = res; \
		} \
		[[nodiscard]] TYPE get_resource() \
		{ \
			return m_resource_; \
		} \
	private: \
		TYPE m_resource_ { nullptr }; \
	}

#define VULKAN_RHI_RESOURCE_DECLARE_REF(PARENT, SELF, TYPE, REF) \
	class SELF : public PARENT \
	{ \
	public: \
		void set_resource(const TYPE res) \
		{ \
			m_resource_ = res; \
		} \
		[[nodiscard]] TYPE REF get_resource() \
		{ \
			return m_resource_; \
		} \
	private: \
		TYPE m_resource_ { nullptr }; \
	} 

namespace source_module::rhi
{
	VULKAN_RHI_RESOURCE_DECLARE(rhi_buffer, vulkan_rhi_buffer, VkBuffer);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_buffer_view, vulkan_rhi_buffer_view, VkBufferView);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_command_buffer, vulkan_rhi_command_buffer, VkCommandBuffer);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_command_pool, vulkan_rhi_command_pool, VkCommandPool);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_descriptor_pool, vulkan_rhi_descriptor_pool, VkDescriptorPool);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_descriptor_set, vulkan_rhi_descriptor_set, VkDescriptorSet);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_descriptor_set_layout, vulkan_rhi_descriptor_set_layout, VkDescriptorSetLayout);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_device, vulkan_rhi_device, VkDevice);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_device_memory, vulkan_rhi_device_memory, VkDeviceMemory);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_event, vulkan_rhi_event, VkEvent);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_fence, vulkan_rhi_fence, VkFence);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_frame_buffer, vulkan_rhi_frame_buffer, VkFramebuffer);
	VULKAN_RHI_RESOURCE_DECLARE_REF(rhi_image, vulkan_rhi_image, VkImage, &);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_image_view, vulkan_rhi_image_view, VkImageView);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_instance, vulkan_rhi_instance, VkInstance);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_queue, vulkan_rhi_queue, VkQueue);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_physical_device, vulkan_rhi_physical_device, VkPhysicalDevice);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_pipeline, vulkan_rhi_pipeline, VkPipeline);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_pipeline_cache, vulkan_rhi_pipeline_cache, VkPipeline);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_pipeline_layout, vulkan_rhi_pipeline_layout, VkPipelineLayout);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_render_pass, vulkan_rhi_render_pass, VkRenderPass);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_sampler, vulkan_rhi_sampler, VkSampler);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_semaphore, vulkan_rhi_semaphore, VkSemaphore);
	VULKAN_RHI_RESOURCE_DECLARE(rhi_shader, vulkan_rhi_shader, VkShaderModule);
}
