#pragma once
#include <vulkan/vulkan_core.h>

namespace source_runtime
{
	class vulkan_util
	{
	public:
		static uint32_t find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags property_flags);
		static VkShaderModule create_shader_module(VkDevice device, const std::vector<unsigned char>& shader_code);
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
		static VkSampler get_or_create_mipmap_sampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t width, uint32_t height);
		static VkSampler get_or_create_nearest_sampler(VkPhysicalDevice physical_device, VkDevice device);
		static VkSampler get_or_create_linear_sampler(VkPhysicalDevice physical_device, VkDevice device);
	private:
		static std::unordered_map<uint32_t, VkSampler> m_mipmap_sampler_map_;
		static VkSampler m_nearest_sampler_;
		static VkSampler m_linear_sampler_;
	};
}
