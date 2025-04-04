#pragma once
#include <vulkan/vulkan_core.h>

#include "vulkan_rhi_resource.h"
#include "Modules/rhi/rhi_module.h"

namespace source_runtime
{
	class vulkan_util
	{
	public:
		static uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags property_flags);
		static VkShaderModule create_shader_module(VkDevice device, const std::vector<unsigned char>& shader_code);
		static void create_buffer(VkPhysicalDevice physical_device,
			VkDevice device,
			VkDeviceSize size,
			VkBufferUsageFlags usage,
			VkMemoryPropertyFlags property,
			VkBuffer& buffer,
			VkDeviceMemory& device_memory);
		static void create_buffer_and_initialize(VkDevice device,
			VkPhysicalDevice physical_device,
			VkBufferUsageFlags usage_flags,
			VkMemoryPropertyFlags memory_property_flags,
			VkBuffer* buffer,
			VkDeviceMemory* memory,
			VkDeviceSize size,
			const void* data = nullptr,
			int data_size = 0);
		static void copy_buffer(source_module::rhi::rhi_module* rhi, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size);
		static void create_image(VkPhysicalDevice physical_device, 
			VkDevice device, 
			uint32_t image_width, 
			uint32_t image_height, 
			VkFormat format, 
			VkImageTiling 
			image_tiling, 
			VkImageUsageFlags image_usage_flags, 
			VkMemoryPropertyFlags memory_property_flags, 
			VkImage& image, 
			VkDeviceMemory& memory, 
			VkImageCreateFlags image_create_flags, 
			uint32_t array_layers, 
			uint32_t mip_levels);
		static VkImageView create_image_view(VkDevice device, const VkImage& image, VkFormat format, VkImageAspectFlags image_aspect_flags, VkImageViewType view_type, uint32_t layout_count, uint32_t mip_levels);
		static void create_global_image(
			source_module::rhi::rhi_module* rhi, VkImage& image, VkImageView& image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, void* texture_image_pixels,
		                                source_module::render::rhi_format texture_image_format,
		                                uint32_t mip_levels = 0);
		static void create_cube_map(source_module::rhi::rhi_module* rhi, 
		                            VkImage& image, 
		                            VkImageView& image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, std::array<void*, 6> texture_image_pixels, source_module::render::rhi_format texture_image_format, uint32_t mip_levels);
		static void transition_image_layout(source_module::rhi::rhi_module* rhi, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t layer_count, uint32_t mip_levels, VkImageAspectFlags aspect_mask_bits);
		static void copy_buffer_to_image(source_module::rhi::rhi_module* rhi, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count);
		static void gen_mipmap_image(source_module::rhi::rhi_module* rhi, VkImage image, uint32_t width, uint32_t height, uint32_t mip_levels);
		static VkSampler get_or_create_mipmap_sampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t width, uint32_t height);
		static VkSampler get_or_create_nearest_sampler(VkPhysicalDevice physical_device, VkDevice device);
		static VkSampler get_or_create_linear_sampler(VkPhysicalDevice physical_device, VkDevice device);
	private:
		static std::unordered_map<uint32_t, VkSampler> m_mipmap_sampler_map_;
		static VkSampler m_nearest_sampler_;
		static VkSampler m_linear_sampler_;
	};
}
