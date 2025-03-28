#include "vulkan_util.h"

namespace source_runtime
{
	std::unordered_map<uint32_t, VkSampler> vulkan_util::m_mipmap_sampler_map_;
	VkSampler vulkan_util::m_nearest_sampler_ = VK_NULL_HANDLE;
	VkSampler vulkan_util::m_linear_sampler_ = VK_NULL_HANDLE;

	uint32_t vulkan_util::find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags property_flags)
	{
		VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);
		for (int i = 0; i < physical_device_memory_properties.memoryTypeCount; ++i)
		{
			if (type_filter & (1 << i) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
			{
				return i;
			}
		}
		LOG_ERROR("find_memory_type error");
		return 0;
	}

	VkShaderModule vulkan_util::create_shader_module(VkDevice device, const std::vector<unsigned char>& shader_code)
	{
		VkShaderModuleCreateInfo shader_module_create_info{};
		shader_module_create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		shader_module_create_info.codeSize = shader_code.size();
		shader_module_create_info.pCode = reinterpret_cast<const uint32_t*>(shader_code.data());

		VkShaderModule shader_module;
		if (vkCreateShaderModule(device, &shader_module_create_info, nullptr, &shader_module) != VK_SUCCESS)
		{
			return VK_NULL_HANDLE;
		}
		return shader_module;
	}

	void vulkan_util::create_image(const VkPhysicalDevice physical_device, 
		const VkDevice device, 
		const uint32_t image_width, 
		const uint32_t image_height, 
		const VkFormat format, 
		const VkImageTiling image_tiling, 
		const VkImageUsageFlags image_usage_flags, 
		const VkMemoryPropertyFlags memory_property_flags, 
		VkImage& image, 
		VkDeviceMemory& memory, 
		const VkImageCreateFlags image_create_flags, 
		const uint32_t array_layers, 
		const uint32_t mip_levels)
	{
		const VkImageCreateInfo image_create_info
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
			.flags = image_create_flags,
			.imageType = VK_IMAGE_TYPE_2D,
			.format = format,
			.extent = 
			{
				.width = image_width,
				.height = image_height,
				.depth = 1,
			},
			.mipLevels = mip_levels,
			.arrayLayers = array_layers,
			.samples = VK_SAMPLE_COUNT_1_BIT,
			.tiling = image_tiling,
			.usage = image_usage_flags,
			.sharingMode = VK_SHARING_MODE_EXCLUSIVE,
			.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		};

		if (vkCreateImage(device, &image_create_info, nullptr, &image) != VK_SUCCESS)
		{
			LOG_ERROR("failed to create image!");
			return;
		}

		VkMemoryRequirements memory_requirements;
		vkGetImageMemoryRequirements(device, image, &memory_requirements);

		VkMemoryAllocateInfo allocate_info
		{
			.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
			.allocationSize = memory_requirements.size,
			.memoryTypeIndex = find_memory_type(physical_device, memory_requirements.memoryTypeBits, memory_property_flags)
		};

		if (vkAllocateMemory(device, &allocate_info, nullptr, &memory) != VK_SUCCESS)
		{
			LOG_ERROR("failed to allocate image memory!");
			return;
		}

		vkBindImageMemory(device, image, memory, 0);
	}

	VkImageView vulkan_util::create_image_view(const VkDevice device, const VkImage& image, const VkFormat format, const VkImageAspectFlags image_aspect_flags, const VkImageViewType view_type, const uint32_t layout_count, const uint32_t mip_levels)
	{
		const VkImageViewCreateInfo image_view_create_info
		{
			.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
			.image = image,
			.viewType = view_type,
			.format = format,
			.subresourceRange = 
			{
				.aspectMask = image_aspect_flags,
				.baseMipLevel = 0,
				.levelCount = mip_levels,
				.baseArrayLayer = 0,
				.layerCount = layout_count
			}
		};

		VkImageView image_view;
		if (vkCreateImageView(device, &image_view_create_info, nullptr, &image_view) != VK_SUCCESS)
		{
			return image_view;
		}
		return image_view;
	}

	VkSampler vulkan_util::get_or_create_mipmap_sampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0)
		{
			LOG_ERROR("width <= 0 || height <= 0");
		}

		VkSampler sampler;
		uint32_t mip_levels = floor(log2(std::max(width, height))) + 1;
		auto find_sampler = m_mipmap_sampler_map_.find(mip_levels);
		if (find_sampler != m_mipmap_sampler_map_.end())
		{
			return find_sampler->second;
		}
		VkPhysicalDeviceProperties physical_device_properties{};
		vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

		VkSamplerCreateInfo sampler_create_info{};

		sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		sampler_create_info.magFilter = VK_FILTER_LINEAR;
		sampler_create_info.minFilter = VK_FILTER_LINEAR;
		sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
		sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		sampler_create_info.anisotropyEnable = VK_TRUE;
		sampler_create_info.maxAnisotropy = physical_device_properties.limits.maxSamplerAnisotropy;
		sampler_create_info.compareEnable = VK_FALSE;
		sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
		sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
		sampler_create_info.unnormalizedCoordinates = VK_FALSE;
		sampler_create_info.maxLod = mip_levels - 1;

		if (vkCreateSampler(device, &sampler_create_info, nullptr, &m_linear_sampler_) != VK_SUCCESS)
		{
			LOG_ERROR("vk create sampler");
		}

		m_mipmap_sampler_map_.insert(std::make_pair(mip_levels, sampler));

		return sampler;
	}

	VkSampler vulkan_util::get_or_create_nearest_sampler(VkPhysicalDevice physical_device, VkDevice device)
	{
		if (m_nearest_sampler_ == VK_NULL_HANDLE)
		{
			VkPhysicalDeviceProperties physical_device_properties{};
			vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

			VkSamplerCreateInfo sampler_create_info{};

			sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler_create_info.magFilter = VK_FILTER_NEAREST;
			sampler_create_info.minFilter = VK_FILTER_NEAREST;
			sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler_create_info.mipLodBias = 0.0f;
			sampler_create_info.anisotropyEnable = VK_FALSE;
			sampler_create_info.maxAnisotropy = physical_device_properties.limits.maxSamplerAnisotropy;
			sampler_create_info.compareEnable = VK_FALSE;
			sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
			sampler_create_info.minLod = 0.0f;
			sampler_create_info.maxLod = 8.0f;
			sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampler_create_info.unnormalizedCoordinates = VK_FALSE;

			if (vkCreateSampler(device, &sampler_create_info, nullptr, &m_linear_sampler_) != VK_SUCCESS)
			{
				LOG_ERROR("vk create sampler");
			}
		}
		return m_nearest_sampler_;
	}

	VkSampler vulkan_util::get_or_create_linear_sampler(VkPhysicalDevice physical_device, VkDevice device)
	{
		if (m_linear_sampler_ == VK_NULL_HANDLE)
		{
			VkPhysicalDeviceProperties physical_device_properties {};
			vkGetPhysicalDeviceProperties(physical_device, &physical_device_properties);

			VkSamplerCreateInfo sampler_create_info{};

			sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			sampler_create_info.magFilter = VK_FILTER_LINEAR;
			sampler_create_info.minFilter = VK_FILTER_LINEAR;
			sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			sampler_create_info.mipLodBias = 0.0f;
			sampler_create_info.anisotropyEnable = VK_FALSE;
			sampler_create_info.maxAnisotropy = physical_device_properties.limits.maxSamplerAnisotropy;
			sampler_create_info.compareEnable = VK_FALSE;
			sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
			sampler_create_info.minLod = 0.0f;
			sampler_create_info.maxLod = 8.0f;
			sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			sampler_create_info.unnormalizedCoordinates = VK_FALSE;

			if (vkCreateSampler(device, &sampler_create_info, nullptr, &m_linear_sampler_) != VK_SUCCESS)
			{
				LOG_ERROR("vk create sampler");
			}
		}

		return m_linear_sampler_;
	}

}


 