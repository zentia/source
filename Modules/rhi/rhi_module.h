#pragma once

#define GLFW_INCLUDE_VULKAN
#include "rhi_struct.h"
#include "Runtime/RenderCore/WindowSystem.h"
#include <vk_mem_alloc.h>

namespace source_runtime
{
	struct rhi_init_info
	{
		std::shared_ptr<Source::WindowSystem> window_system;
	};

	class rhi
	{
	public:
		virtual ~rhi() = 0;

		virtual void initialize(rhi_init_info init_info) = 0;
		virtual void prepare_context() = 0;

		virtual bool is_point_light_shadow_enabled() = 0;

		virtual bool allocate_command_buffers(const rhi_command_buffer_allocator_info* allocator_info, rhi_command_buffer*& command_buffer) = 0;
		virtual bool allocate_descriptor_sets(const rhi_descriptor_set_allocator_info* allocator_info, rhi_descriptor_set*& descriptor_set) = 0;
		virtual void create_swap_chain() = 0;
		virtual void recreate_swap_chain() = 0;
		virtual void create_swap_chain_image_views() = 0;
		virtual void create_frame_buffer_image_view() = 0;
		virtual rhi_sampler* get_or_create_default_sampler(rhi_default_sampler_type type) = 0;
		virtual rhi_sampler* get_or_create_mipmap_sampler(uint32_t width, uint32_t height) = 0;
		virtual rhi_shader* create_shader_module(const std::vector<unsigned char>& shader_code) = 0;
		virtual void create_buffer(rhi_device_size size, rhi_buffer_usage_flags usage, rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory) = 0;
		virtual void create_buffer_and_initialize(rhi_buffer_usage_flags usage, rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory, rhi_device_size size, void* data = nullptr, int data_size = 0) = 0;
		virtual bool create_buffer_vma(VmaAllocator allocator, const rhi_buffer_create_info* buffer_create_info, const VmaAllocationCreateInfo* allocation_create_info, rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) = 0;
		virtual bool create_buffer_with_alignment_vma(VmaAllocator allocator, const rhi_buffer_create_info* buffer_create_info, VmaAllocationCreateInfo* allocation_create_info, rhi_device_size min_alignment, rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) = 0;
		virtual void copy_buffer(rhi_buffer* src_buffer, rhi_buffer* dst_buffer, rhi_device_size src_offset, rhi_device_size dst_offset, rhi_device_size size) = 0;
		virtual void create_image(uint32_t image_width, uint32_t image_height, rhi_format format, rhi_image_aspect_flags image_aspect_flags, rhi_image_view view_type, uint32_t layout_count, uint32_t mip_levels, rhi_image_view*& image_view) = 0;
		virtual void create_image_view(rhi_image* image, rhi_format format, rhi_image_aspect_flags image_aspect_flags, rhi_image_view_type view_type, uint32_t layout_count, uint32_t mip_levels, rhi_image_view*& image_view) = 0;
		virtual void create_global_image(rhi_image*& image, rhi_image_view*& image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, void* texture_image_pixels, rhi_format texture_image_format, uint32_t mip_levels = 0) = 0;
		virtual void create_cube_map(rhi_image*& image, rhi_image_view*& image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, std::array<void*, 6> texture_image_pixels, rhi_format texture_image_format, uint32_t mip_levels) = 0;
		virtual void create_command_pool() = 0;
		virtual bool create_command_pool(const rhi_command_pool_create_info* create_info, rhi_command_pool*& command_pool) = 0;
		virtual bool create_descriptor_pool(const rhi_descriptor_pool_create_info* create_info, rhi_descriptor_pool*& descriptor_pool) = 0;
		virtual bool create_descriptor_set_layout(const rhi_descriptor_set_layout_create_info* create_info, rhi_descriptor_set_layout*& set_layout) = 0;
		virtual bool create_fence(const rhi_fence_create_info* create_info, rhi_fence*& fence) = 0;
		virtual bool create_frame_buffer(const rhi_frame_buffer_create) = 0;
		// command and command write
		virtual bool wait_for_fences_pfn(uint32_t fence_count, rhi_fence* const* fence, rhi_bool32 wait_all, uint64_t timeout) = 0;

		// query
		virtual void get_physical_device_properties(rhi_physical_device_properties* properties) = 0;

		// command write
		virtual rhi_command_buffer* begin_single_time_commands() = 0;
		virtual void end_single_time_commands(rhi_command_buffer* command_buffer) = 0;
		virtual bool prepare_before_pass(std::function<void()> pass_update_after_recreate_swap_chain) = 0;
		virtual void submit_rendering(std::function<void()> pass_update_after_recreate_swap_chain) = 0;
		virtual void push_event(rhi_command_buffer* command_buffer, const char* name, const float* color) = 0;
		virtual void pop_event(rhi_command_buffer* command_buffer) = 0;

		// destroy
		virtual void clear() = 0;
		virtual void clear_swap_chain() = 0;
		virtual void destroy_default_sampler(rhi_default_sampler_type type) = 0;
		virtual void destroy_mipmap_sampler() = 0;
		virtual void destroy_shader_module(rhi_shader* shader) = 0;
		virtual void destroy_semaphore(rhi_semaphore* semaphore) = 0;
		virtual void destroy_sampler(rhi_sampler* sampler) = 0;
		virtual void destroy_instance(rhi_instance* instance) = 0;
		virtual void destroy_image_view(rhi_image_view* image_view) = 0;
		virtual void destroy_image(rhi_image* image) = 0;
		virtual void destroy_frame_buffer(rhi_frame_buffer* frame_buffer) = 0;
		virtual void destroy_fence(rhi_fence* fence) = 0;
		virtual void destroy_device() = 0;
		virtual void destroy_command_pool(rhi_command_pool* command_pool) = 0;
		virtual void destroy_buffer(rhi_buffer*& buffer) = 0;
		virtual void free_command_buffer(rhi_command_pool* command_pool, uint32_t command_buffer_count, rhi_command_buffer* command_buffer) = 0;
		// memory
		virtual void free_memory(rhi_device_memory*& memory) = 0;

		// semaphores
		virtual rhi_semaphore*& get_texture_copy_semaphore(uint32_t index) = 0;
	};

	inline rhi::~rhi() = default;
}
