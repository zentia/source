#pragma once

#define GLFW_INCLUDE_VULKAN
#include "rhi_struct.h"
#include "vk_mem_alloc.h"
#include "Modules/window/window_module.h"
#include <memory>

namespace source_module::rhi
{
	struct rhi_init_info
	{
		std::shared_ptr<source_runtime::window_module> window_module;
	};

	class rhi
	{
	public:
		virtual ~rhi() = 0;

		virtual void initialize(rhi_init_info init_info) = 0;
		virtual void prepare_context() = 0;

		virtual bool is_point_light_shadow_enabled() = 0;

		virtual bool allocate_command_buffers(const source_runtime::rhi_command_buffer_allocator_info* allocator_info, source_runtime::rhi_command_buffer*& command_buffer) = 0;
		virtual bool allocate_descriptor_sets(const source_runtime::rhi_descriptor_set_allocator_info* allocator_info, source_runtime::rhi_descriptor_set*& descriptor_set) = 0;
		virtual void create_swap_chain() = 0;
		virtual void recreate_swap_chain() = 0;
		virtual void create_swap_chain_image_views() = 0;
		virtual void create_frame_buffer_image_view() = 0;
		virtual source_runtime::rhi_sampler* get_or_create_default_sampler(source_runtime::rhi_default_sampler_type type) = 0;
		virtual source_runtime::rhi_sampler* get_or_create_mipmap_sampler(uint32_t width, uint32_t height) = 0;
		virtual source_runtime::rhi_shader* create_shader_module(const std::vector<unsigned char>& shader_code) = 0;
		virtual void create_buffer(source_runtime::rhi_device_size size, source_runtime::rhi_buffer_usage_flags usage, source_runtime::rhi_memory_property_flags properties, source_runtime::rhi_buffer*& buffer, source_runtime::rhi_device_memory*& buffer_memory, void* data = nullptr, int data_size = 0) = 0;
		virtual bool create_buffer_vma(VmaAllocator allocator, const source_runtime::rhi_buffer_create_info* buffer_create_info, const VmaAllocationCreateInfo* allocation_create_info, source_runtime::rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) = 0;
		virtual bool create_buffer_with_alignment_vma(VmaAllocator allocator, 
			const source_runtime::rhi_buffer_create_info* buffer_create_info, VmaAllocationCreateInfo* allocation_create_info, source_runtime::rhi_device_size min_alignment, source_runtime::rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) = 0;
		virtual void copy_buffer(source_runtime::rhi_buffer* src_buffer, source_runtime::rhi_buffer* dst_buffer, source_runtime::rhi_device_size src_offset, source_runtime::rhi_device_size dst_offset, source_runtime::rhi_device_size size) = 0;
		virtual void create_image(uint32_t image_width, uint32_t image_height, source_runtime::rhi_format format,
		                          source_runtime::rhi_image_tiling image_tiling,
		                          source_runtime::rhi_image_usage_flags image_usage_flags,
		                          source_runtime::rhi_memory_property_flags memory_property_flags,
		                          source_runtime::rhi_image*& image,
		                          source_runtime::rhi_device_memory*& memory,
		                          source_runtime::rhi_image_create_flags image_create_flags,
			uint32_t array_layers,
			uint32_t mip_levels) = 0;
		virtual void create_image_view(source_runtime::rhi_image* image, source_runtime::rhi_format format, source_runtime::rhi_image_aspect_flags image_aspect_flags, source_runtime::rhi_image_view_type view_type, uint32_t layout_count, uint32_t mip_levels, source_runtime::rhi_image_view*& image_view) = 0;
		virtual void create_global_image(source_runtime::rhi_image*& image, source_runtime::rhi_image_view*& image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, void* texture_image_pixels, source_runtime::rhi_format texture_image_format, uint32_t mip_levels = 0) = 0;
		virtual void create_cube_map(source_runtime::rhi_image*& image, source_runtime::rhi_image_view*& image_view, VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, std::array<void*, 6> texture_image_pixels, source_runtime::rhi_format texture_image_format, uint32_t mip_levels) = 0;
		virtual void create_command_pool() = 0;
		virtual bool create_command_pool(const source_runtime::rhi_command_pool_create_info* create_info, source_runtime::rhi_command_pool*& command_pool) = 0;
		virtual bool create_descriptor_pool(const source_runtime::rhi_descriptor_pool_create_info* create_info, source_runtime::rhi_descriptor_pool*& descriptor_pool) = 0;
		virtual bool create_descriptor_set_layout(const source_runtime::rhi_descriptor_set_layout_create_info* create_info, source_runtime::rhi_descriptor_set_layout*& set_layout) = 0;
		virtual bool create_fence(const source_runtime::rhi_fence_create_info* create_info, source_runtime::rhi_fence*& fence) = 0;
		virtual bool create_frame_buffer(const source_runtime::rhi_frame_buffer_create_info* create_info, source_runtime::rhi_frame_buffer*& frame_buffer) = 0;
		virtual bool create_graphics_pipelines(source_runtime::rhi_pipeline_cache* pipeline_cache, uint32_t create_info_count, const source_runtime::rhi_graphics_pipeline_create_info* create_info, source_runtime::rhi_pipeline*& pipeline) = 0;
		virtual bool create_compute_pipelines() = 0;
		virtual bool create_pipeline_layout() = 0;
		virtual bool create_render_pass() = 0;
		virtual bool create_sampler() = 0;
		virtual bool create_semaphore() = 0;

		// command and command write
		virtual bool wait_for_fences(uint32_t fence_count, source_runtime::rhi_fence* const* fence, source_runtime::rhi_bool32 wait_all, uint64_t timeout) = 0;
		virtual bool reset_fences(uint32_t fence_count, source_runtime::rhi_fence* const* fence) = 0;
		virtual bool reset_command_pool(source_runtime::rhi_command_pool* command_pool, source_runtime::rhi_command_pool_reset_flags flags) = 0;
		virtual bool begin_command_buffer(source_runtime::rhi_command_buffer* command_buffer, const source_runtime::rhi_command_buffer_begin_info* begin_info) = 0;
		virtual bool end_command_buffer(source_runtime::rhi_command_buffer* command_buffer) = 0;
		virtual void cmd_begin_render_pass(source_runtime::rhi_command_buffer* command_buffer, const source_runtime::rhi_render_pass_begin_info* render_pass_begin, source_runtime::rhi_sub_pass_contents contents) = 0;
		virtual void cmd_next_sub_pass(source_runtime::rhi_command_buffer* command_buffer, source_runtime::rhi_sub_pass_contents contents) = 0;
		virtual void cmd_end_render_pass(source_runtime::rhi_command_buffer* command_buffer) = 0;
		virtual void cmd_bind_pipeline(source_runtime::rhi_command_buffer* command_buffer, source_runtime::rhi_pipeline_bind_point pipeline_bind_point, source_runtime::rhi_pipeline* pipeline) = 0;
		virtual void cmd_set_viewport(source_runtime::rhi_command_buffer* command_buffer, uint32_t first_viewport, uint32_t viewport_count, const source_runtime::rhi_viewport* viewports) = 0;
		virtual void cmd_set_scissor(source_runtime::rhi_command_buffer* command_buffer, uint32_t first_scissor, uint32_t scissor_count, const source_runtime::rhi_rect_2d* scissors) = 0;
		virtual void cmd_bind_vertex_buffers(source_runtime::rhi_command_buffer* command_buffer, uint32_t first_binding, uint32_t binding_count, source_runtime::rhi_buffer* const* buffers, const source_runtime::rhi_device_size* offsets) = 0;
		virtual void cmd_bind_index_buffer(source_runtime::rhi_command_buffer* command_buffer, source_runtime::rhi_buffer* buffer, source_runtime::rhi_device_size offset, source_runtime::rhi_index_type index_type) = 0;
		virtual void cmd_bind_descriptor_sets(source_runtime::rhi_command_buffer* command_buffer,
		                                      source_runtime::rhi_pipeline_bind_point pipeline_bind_point,
		                                      source_runtime::rhi_pipeline_layout* layout,
		                                      uint32_t first_set,
		                                      uint32_t descriptor_set_count,
		                                      const source_runtime::rhi_descriptor_set* const* descriptors,
		                                      uint32_t dynamic_offset_count,
		                                      const uint32_t* dynamic_offsets) = 0;
		virtual void cmd_draw_indexed(source_runtime::rhi_command_buffer* command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) = 0;
		virtual void cmd_clear_attachments(source_runtime::rhi_command_buffer* command_buffer, uint32_t attachment_count, const source_runtime::rhi_clear_attachment* attachment, uint32_t rect_count, const source_runtime::rhi_clear_rect* rects) = 0;

		// query
		virtual void get_physical_device_properties(source_runtime::rhi_physical_device_properties* properties) = 0;
		virtual [[nodiscard]] source_runtime::rhi_command_buffer* get_current_command_buffer() const = 0;
		virtual source_runtime::rhi_command_buffer* const* get_command_buffer_list() const = 0;
		virtual source_runtime::rhi_command_pool* get_command_pool() const = 0;
		virtual source_runtime::rhi_descriptor_pool* get_descriptor_pool() const = 0;
		virtual source_runtime::rhi_fence* const* get_fence_list() const = 0;
		virtual source_runtime::queue_family_indices get_queue_family_indices() const = 0;
		virtual source_runtime::rhi_queue* get_graphics_queue() const = 0;
		virtual source_runtime::rhi_queue* get_compute_queue() const = 0;
		virtual source_runtime::rhi_swap_chain_desc get_swap_chain_info() = 0;
		virtual source_runtime::rhi_depth_image_desc get_depth_image_info() = 0;
		virtual uint8_t get_max_frames_in_flight() const = 0;
		virtual uint8_t get_current_frame_index() const = 0;
		virtual void set_current_frame_index(uint8_t index) = 0;
		// command write
		virtual source_runtime::rhi_command_buffer* begin_single_time_commands() = 0;
		virtual void end_single_time_commands(source_runtime::rhi_command_buffer* command_buffer) = 0;
		virtual bool prepare_before_pass(std::function<void()> pass_update_after_recreate_swap_chain) = 0;
		virtual void submit_rendering(std::function<void()> pass_update_after_recreate_swap_chain) = 0;
		virtual void push_event(source_runtime::rhi_command_buffer* command_buffer, const char* name, const float* color) = 0;
		virtual void pop_event(source_runtime::rhi_command_buffer* command_buffer) = 0;

		// destroy
		virtual void clear() = 0;
		virtual void clear_swap_chain() = 0;
		virtual void destroy_default_sampler(source_runtime::rhi_default_sampler_type type) = 0;
		virtual void destroy_mipmap_sampler() = 0;
		virtual void destroy_shader_module(source_runtime::rhi_shader* shader) = 0;
		virtual void destroy_semaphore(source_runtime::rhi_semaphore* semaphore) = 0;
		virtual void destroy_sampler(source_runtime::rhi_sampler* sampler) = 0;
		virtual void destroy_instance(source_runtime::rhi_instance* instance) = 0;
		virtual void destroy_image_view(source_runtime::rhi_image_view* image_view) = 0;
		virtual void destroy_image(source_runtime::rhi_image* image) = 0;
		virtual void destroy_frame_buffer(source_runtime::rhi_frame_buffer* frame_buffer) = 0;
		virtual void destroy_fence(source_runtime::rhi_fence* fence) = 0;
		virtual void destroy_device() = 0;
		virtual void destroy_command_pool(source_runtime::rhi_command_pool* command_pool) = 0;
		virtual void destroy_buffer(source_runtime::rhi_buffer*& buffer) = 0;
		virtual void free_command_buffer(source_runtime::rhi_command_pool* command_pool, uint32_t command_buffer_count, source_runtime::rhi_command_buffer* command_buffer) = 0;
		// memory
		virtual void free_memory(source_runtime::rhi_device_memory*& memory) = 0;

		// semaphores
		virtual source_runtime::rhi_semaphore*& get_texture_copy_semaphore(uint32_t index) = 0;
	};

	inline rhi::~rhi() = default;
}
