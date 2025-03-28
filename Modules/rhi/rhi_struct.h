#pragma once
#include <vulkan/vulkan_core.h>

#include "Modules/render/render_type.h"


#define RHI_RESOURCE_DECLARE(SELF, TYPE) \
	class SELF \
	{ \
	} 

namespace source_runtime
{
	RHI_RESOURCE_DECLARE(rhi_buffer);
	RHI_RESOURCE_DECLARE(rhi_buffer_view);
	RHI_RESOURCE_DECLARE(rhi_command_buffer);
	RHI_RESOURCE_DECLARE(rhi_command_pool);
	RHI_RESOURCE_DECLARE(rhi_descriptor_pool);
	RHI_RESOURCE_DECLARE(rhi_descriptor_set);
	RHI_RESOURCE_DECLARE(rhi_descriptor_set_layout);
	RHI_RESOURCE_DECLARE(rhi_device);
	RHI_RESOURCE_DECLARE(rhi_device_memory);
	RHI_RESOURCE_DECLARE(rhi_event);
	RHI_RESOURCE_DECLARE(rhi_fence);
	RHI_RESOURCE_DECLARE(rhi_frame_buffer);
	RHI_RESOURCE_DECLARE(rhi_image);
	RHI_RESOURCE_DECLARE(rhi_image_view);
	RHI_RESOURCE_DECLARE(rhi_instance);
	RHI_RESOURCE_DECLARE(rhi_queue);
	RHI_RESOURCE_DECLARE(rhi_physical_device);
	RHI_RESOURCE_DECLARE(rhi_pipeline);
	RHI_RESOURCE_DECLARE(rhi_pipeline_cache);
	RHI_RESOURCE_DECLARE(rhi_pipeline_layout);
	RHI_RESOURCE_DECLARE(rhi_render_pass);
	RHI_RESOURCE_DECLARE(rhi_sampler);
	RHI_RESOURCE_DECLARE(rhi_semaphore);
	RHI_RESOURCE_DECLARE(rhi_shader);
	
	struct rhi_memory_barrier
	{
		rhi_structure_type s_type;
	};
	struct rhi_offset_2d
	{
		int32_t x;
		int32_t y;
	};
	struct rhi_extent_2d
	{
		uint32_t width;
		uint32_t height;
	};
	struct rhi_rect_2d
	{
		rhi_offset_2d offset;
		rhi_extent_2d extend;
	};
	struct rhi_viewport
	{
		float x;
		float y;
		float width;
		float height;
		float min_depth;
		float max_depth;
	};

	struct queue_family_indices
	{
		std::optional<uint32_t> graphics_family;
		std::optional<uint32_t> present_family;
		std::optional<uint32_t> compute_family;

		[[nodiscard]]bool is_complete() const { return graphics_family.has_value() && present_family.has_value() && compute_family.has_value(); }
	};

	struct rhi_command_buffer_allocator_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_command_pool* command_pool;
		rhi_command_buffer_level level;
		uint32_t command_buffer_count;
	};

	struct rhi_descriptor_set_allocator_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_descriptor_pool* descriptor_pool;
		uint32_t descriptor_set_count;
		rhi_descriptor_set_layout** set_layouts;
	};

	struct rhi_buffer_create_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_buffer_create_flags flags;
		rhi_device_size size;
		rhi_buffer_usage_flags usage;
		rhi_shading_mode shading_mode;
		uint32_t queue_family_index_count;
		const uint32_t* queue_family_indices;
	};

	struct swap_chain_support_details
	{
		// 表面能力，交换链中图形的最小最大数量，最小最大尺寸，每个图像的最大层数
		VkSurfaceCapabilitiesKHR capabilities;
		// 图像格式和颜色空间
		std::vector<VkSurfaceFormatKHR> format;
		// 呈现模式
		std::vector<VkPresentModeKHR> present_mode;
	};

	struct rhi_physical_device_limits
	{
		uint32_t max_image_dimension_1d;
		uint32_t max_image_dimension_2d;
		uint32_t max_image_dimension_3d;
		uint32_t max_image_dimension_cube;
		uint32_t max_image_array_layers;
		uint32_t max_texel_buffer_elements;
		uint32_t max_uniform_buffer_range;
		uint32_t mxa_storage_buffer_range;
		uint32_t max_push_constants_size;
		uint32_t max_memory_allocation_count;
		uint32_t max_sampler_allocation_count;
		rhi_device_size buffer_image_granularity;
		rhi_device_size sparse_address_space_size;
		uint32_t max_bound_descriptor_sets;
		uint32_t max_per_stage_descriptor_samplers;
		uint32_t max_per_stage_descriptor_uniform_buffers;
		uint32_t max_per_stage_descriptor_storage_buffers;
		uint32_t max_per_stage_descriptor_sampled_images;
		uint32_t max_per_stage_descriptor_storage_images;

	};

	struct rhi_physical_device_sparse_properties
	{
		rhi_bool32 residency_standard_2d_block_shape;
		rhi_bool32 residency_standard_2d_multi_sample_block_shape;
		rhi_bool32 residency_standard_3d_block_shape;
		rhi_bool32 residency_aligned_mip_size;
		rhi_bool32 residency_non_resident_strict;
	};
	struct rhi_physical_device_properties
	{
		uint32_t api_version;
		uint32_t driver_version;
		uint32_t vendor_id;
		uint32_t device_id;
		rhi_physical_device_type device_type;
		char device_name[RHI_MAX_PHYSICAL_DEVICE_NAME_SIZE];
		uint8_t pipeline_cache_uuid[RHI_UUID_SIZE];
		rhi_physical_device_limits limits;
		rhi_physical_device_sparse_properties sparse_properties;
	};

	struct rhi_command_pool_create_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_command_pool_create_flags flags;
		uint32_t queue_family_index;
	};
	struct rhi_descriptor_pool_size
	{
		rhi_descriptor_type type;
		uint32_t descriptor_count;
	};
	struct rhi_descriptor_pool_create_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_descriptor_pool_create_flags flags;
		uint32_t max_sets;
		uint32_t pool_size_count;
		const rhi_descriptor_pool_size* pool_size;
	};
	struct rhi_descriptor_set_layout_binding
	{
		uint32_t binding;
		rhi_descriptor_type descriptor;
		uint32_t descriptor_count;
		rhi_shader_stage_flags stage_flags;
		rhi_sampler* const* immutable_samplers = nullptr;
	};
	struct rhi_descriptor_set_layout_create_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_descriptor_set_layout_create_flags flags;
		uint32_t binding_count;
		const rhi_descriptor_set_layout_binding* bindings;
	};
	struct rhi_fence_create_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_fence_create_flags flags;
	};
	struct rhi_frame_buffer_create_info
	{
		rhi_structure_type type;
		const void* next;
		rhi_frame_buffer_create_flags flags;
		rhi_render_pass* render_pass;
		uint32_t attachment_count;
		rhi_image_view* const* attachments;
		uint32_t width;
		uint32_t height;
		uint32_t layers;
	};
}
