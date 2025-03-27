#pragma once
#include "Modules/render/render_type.h"

namespace source_runtime
{
	class rhi_buffer
	{
	public:
		
	};

	class rhi_buffer_view
	{
	public:
		
	};

	class rhi_command_buffer
	{
	public:
		
	};

	class rhi_command_pool
	{
	public:
		
	};

	class rhi_descriptor_pool
	{
	public:
		
	};

	class rhi_descriptor_set
	{
	public:
		
	};

	class rhi_descriptor_set_layout
	{
	public:
		
	};

	class rhi_device
	{
	public:
		
	};

	class rhi_device_memory
	{
	public:
		
	};

	class rhi_event
	{
	public:
		
	};

	class rhi_fence
	{
	public:
		
	};

	class rhi_frame_buffer
	{
	public:
		
	};

	class rhi_image
	{
	public:
		
	};

	class rhi_image_view
	{
	public:
		
	};

	class rhi_instance
	{
	public:
		
	};

	class rhi_queue
	{
	public:
		
	};

	class rhi_physical_device
	{
	public:
		
	};

	class rhi_pipeline
	{
	public:
		
	};

	class rhi_pipeline_cache
	{
	public:
		
	};

	class rhi_pipeline_layout
	{
	public:
		
	};

	class rhi_render_pass
	{
	public:
		
	};

	class rhi_sampler
	{
	public:
		
	};

	class rhi_semaphore
	{
	public:
		
	};

	class rhi_shader
	{
	public:
		
	};

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

		bool is_complete() const { return graphics_family.has_value() && present_family.has_value() && compute_family.has_value(); }
	};

	struct swap_chain_support_details
	{
		VkSurfaceCapabilitiesKHR capabilities;
		std::vector<VkSurfaceFormatKHR> format;
		std::vector<VkPresentModeKHR> present_mode;
	};
}
