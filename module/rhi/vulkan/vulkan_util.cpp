﻿#include "vulkan_util.h"

#include "module/rhi/vulkan/vulkan_rhi.h"

#include "spdlog/spdlog.h"

namespace source_runtime
{
	std::unordered_map<uint32_t, VkSampler> vulkan_util::m_mipmap_sampler_map_;
	VkSampler vulkan_util::m_nearest_sampler_ = VK_NULL_HANDLE;
	VkSampler vulkan_util::m_linear_sampler_ = VK_NULL_HANDLE;

	uint32_t vulkan_util::find_memory_type(const VkPhysicalDevice physical_device, const uint32_t type_filter, const VkMemoryPropertyFlags property_flags)
	{
		VkPhysicalDeviceMemoryProperties physical_device_memory_properties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &physical_device_memory_properties);
		for (uint32_t i = 0; i < physical_device_memory_properties.memoryTypeCount; ++i)
		{
			if (type_filter & (1 << i) && (physical_device_memory_properties.memoryTypes[i].propertyFlags & property_flags) == property_flags)
			{
				return i;
			}
		}
		SPDLOG_ERROR("find_memory_type error");
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

	void vulkan_util::create_buffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags property, VkBuffer& buffer, VkDeviceMemory& device_memory)
	{
		VkBufferCreateInfo buffer_create_info;
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = size;
		buffer_create_info.usage = usage;
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &buffer_create_info, nullptr, &buffer) != VK_SUCCESS)
		{
			SPDLOG_ERROR("vkCreateBuffer failed!");
			return;
		}

		VkMemoryRequirements memory_requirements;
		vkGetBufferMemoryRequirements(device, buffer, &memory_requirements);

		VkMemoryAllocateInfo memory_allocate_info;
		memory_allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		memory_allocate_info.allocationSize = memory_requirements.size;
		memory_allocate_info.memoryTypeIndex = find_memory_type(physical_device, memory_requirements.memoryTypeBits, property);

		if (vkAllocateMemory(device, &memory_allocate_info, nullptr, &device_memory) != VK_SUCCESS)
		{
			SPDLOG_ERROR("vkAllocateMemory failed!");
			return;
		}

		vkBindBufferMemory(device, buffer, device_memory, 0);
	}

	void vulkan_util::create_buffer_and_initialize(const VkDevice device,
		const VkPhysicalDevice physical_device,
		const VkBufferUsageFlags usage_flags,
		const VkMemoryPropertyFlags memory_property_flags,
		VkBuffer* buffer,
		VkDeviceMemory* memory,
		const VkDeviceSize size, 
		const void* data, 
		const int data_size)
	{
		create_buffer(physical_device, device, size, usage_flags, memory_property_flags, *buffer, *memory);
		if (data != nullptr && data_size != 0)
		{
			void* mapped;
			if (vkMapMemory(device, *memory, 0, size, 0, &mapped) != VK_SUCCESS)
			{
				SPDLOG_ERROR("map memory failed!");
				return;
			}
			memcpy(mapped, data, data_size);
			vkUnmapMemory(device, *memory);
		}
	}

	void vulkan_util::copy_buffer(source_module::rhi::rhi_module* rhi, const VkBuffer src_buffer, const VkBuffer dst_buffer, const VkDeviceSize src_offset, const VkDeviceSize dst_offset, const VkDeviceSize size)
	{
		if (rhi == nullptr)
		{
			SPDLOG_ERROR("rhi is nullptr");
			return;
		}
		source_module::rhi::rhi_command_buffer* rhi_command_buffer = dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->begin_single_time_commands();
		const VkCommandBuffer vk_command_buffer = static_cast<source_module::rhi::vulkan_rhi_command_buffer*>(rhi_command_buffer)->get_resource();

		const VkBufferCopy vk_buffer_copy = { src_offset, dst_offset, size };
		vkCmdCopyBuffer(vk_command_buffer, src_buffer, dst_buffer, 1, &vk_buffer_copy);

		dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->end_single_time_commands(rhi_command_buffer);
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
			SPDLOG_ERROR("failed to create image!");
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
			SPDLOG_ERROR("failed to allocate image memory!");
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

	void vulkan_util::transition_image_layout(source_module::rhi::rhi_module* rhi, VkImage image, VkImageLayout old_layout, VkImageLayout new_layout, uint32_t layer_count, uint32_t mip_levels, VkImageAspectFlags aspect_mask_bits)
	{
		if (rhi == nullptr)
		{
			SPDLOG_ERROR("rhi is nullptr");
			return;
		}

		source_module::rhi::rhi_command_buffer* rhi_command_buffer = dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->begin_single_time_commands();
		VkCommandBuffer command_buffer = static_cast<source_module::rhi::vulkan_rhi_command_buffer*>(rhi_command_buffer)->get_resource();

		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = old_layout;
		barrier.newLayout = new_layout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = aspect_mask_bits;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layer_count;

		VkPipelineStageFlags src_stage;
		VkPipelineStageFlags dst_stage;

		// 图像的初始状态，内容未定义。通常在图像创建后立即转换为其他布局
		// 图像用于作为数据目标进行创数操作时的最佳布局
		if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
		{
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		// 图像用于着色器读取时的最佳布局，适用于纹理采样
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		// 图像用作深度/模板进行渲染时的最佳布局
		// 图像用于作为数据源进行传输操作时的最佳布局
		else if (old_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			src_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
			dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

			src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dst_stage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		}
		else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && new_layout == VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL)
		{
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else
		{
			SPDLOG_ERROR("unsupported layout transition!");
			return;
		}

		vkCmdPipelineBarrier(command_buffer, src_stage, dst_stage, 0, 0, nullptr, 0, nullptr, 1, &barrier);

		dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->end_single_time_commands(rhi_command_buffer);
	}

	void vulkan_util::copy_buffer_to_image(source_module::rhi::rhi_module* rhi, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count)
	{
		if (rhi == nullptr)
		{
			SPDLOG_ERROR("rhi is nullptr");
			return;
		}

		source_module::rhi::rhi_command_buffer* rhi_command_buffer = dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->begin_single_time_commands();
		VkCommandBuffer command_buffer = static_cast<source_module::rhi::vulkan_rhi_command_buffer*>(rhi_command_buffer)->get_resource();

		VkBufferImageCopy region;
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layer_count;
		region.imageOffset = { 0,0,0 };
		region.imageExtent = { width, height, 1 };

		vkCmdCopyBufferToImage(command_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->end_single_time_commands(rhi_command_buffer);
	}

	void vulkan_util::gen_mipmap_image(source_module::rhi::rhi_module* rhi, VkImage image, uint32_t width, uint32_t height, uint32_t mip_levels)
	{
		if (rhi == nullptr)
		{
			SPDLOG_ERROR("rhi is nullptr");
			return;
		}

		source_module::rhi::rhi_command_buffer* rhi_command_buffer = dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->begin_single_time_commands();
		VkCommandBuffer command_buffer = static_cast<source_module::rhi::vulkan_rhi_command_buffer*>(rhi_command_buffer)->get_resource();

		for (uint32_t i = 0; i < mip_levels; ++i)
		{
			VkImageBlit image_blit;
			image_blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_blit.srcSubresource.layerCount = 1;
			image_blit.srcSubresource.mipLevel = i - 1;
			image_blit.srcOffsets[1].x = std::max(static_cast<int32_t>(width >> (i - 1)), 1);
			image_blit.srcOffsets[1].y = std::max(static_cast<int32_t>(height >> (i - 1)), 1);
			image_blit.srcOffsets[1].z = 1;

			image_blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			image_blit.dstSubresource.layerCount = 1;
			image_blit.dstSubresource.mipLevel = i;
			image_blit.dstOffsets[1].x = std::max(static_cast<int32_t>(width >> i), 1);
			image_blit.dstOffsets[1].y = std::max(static_cast<int32_t>(height >> i), 1);
			image_blit.dstOffsets[1].z = 1;

			VkImageSubresourceRange mip_sub_range;
			mip_sub_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			mip_sub_range.baseMipLevel = i;
			mip_sub_range.levelCount = 1;
			mip_sub_range.layerCount = 1;

			VkImageMemoryBarrier barrier;
			barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
			barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
			barrier.image = image;
			barrier.subresourceRange = mip_sub_range;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				0,
				0,
				nullptr,
				0,
				nullptr,
				1,
				&barrier);

			vkCmdBlitImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &image_blit, VK_FILTER_LINEAR);

			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer, 
				VK_PIPELINE_STAGE_TRANSFER_BIT, 
				VK_PIPELINE_STAGE_TRANSFER_BIT, 
				0, 
				0, 
				nullptr, 
				0, 
				nullptr, 
				1, 
				&barrier);
		}

		VkImageSubresourceRange mip_sub_range;
		mip_sub_range.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		mip_sub_range.baseMipLevel = 0;
		mip_sub_range.levelCount = mip_levels;
		mip_sub_range.layerCount = 1;

		VkImageMemoryBarrier barrier;
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange = mip_sub_range;

		vkCmdPipelineBarrier(command_buffer,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			0,
			0,
			nullptr,
			0,
			nullptr,
			1,
			&barrier);

		dynamic_cast<source_module::rhi::vulkan_rhi*>(rhi)->end_single_time_commands(rhi_command_buffer);
	}

	VkSampler vulkan_util::get_or_create_mipmap_sampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t width, uint32_t height)
	{
		if (width <= 0 || height <= 0)
		{
			SPDLOG_ERROR("width <= 0 || height <= 0");
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
			SPDLOG_ERROR("vk create sampler");
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
				SPDLOG_ERROR("vk create sampler");
			}
		}
		return m_nearest_sampler_;
	}

	VkSampler vulkan_util::get_or_create_linear_sampler(VkPhysicalDevice physical_device, VkDevice device)
	{
		if (m_linear_sampler_ == VK_NULL_HANDLE)
		{
			VkPhysicalDeviceProperties physical_device_properties{};
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
				SPDLOG_ERROR("vk create sampler");
			}
		}

		return m_linear_sampler_;
	}

}


