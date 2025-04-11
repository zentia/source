#pragma once

#include "module/window/window_module.h"
#include <memory>

#include "multi_gpu.h"
#include "pixel_format.h"
#include "rhi_define.h"
#include "rhi_resources.h"
#include "pipeline/rhi_pipeline.h"

namespace source_module::rhi
{
	struct rhi_init_info
	{
		std::shared_ptr<window::window_module> window_module;
	};

	class rhi_module
	{
	public:
		rhi_module() = default;
		rhi_module(rhi_module& rhi_module) = delete;
		rhi_module(rhi_module&& rhi_module) = delete;
		rhi_module& operator=(rhi_module& rhi_module) = delete;
		rhi_module& operator=(rhi_module&& rhi_module) = delete;
		virtual ~rhi_module() = 0;

		virtual void initialize(rhi_init_info init_info);
		virtual void prepare_context();

		virtual bool is_point_light_shadow_enabled();

		virtual bool allocate_command_buffers(const rhi_command_buffer_allocator_info* allocator_info, rhi_command_buffer*& command_buffer);
		virtual bool allocate_descriptor_sets(const rhi_descriptor_set_allocator_info* allocator_info, rhi_descriptor_set*& descriptor_set);
		virtual void create_swap_chain();
		virtual void recreate_swap_chain();
		virtual void create_swap_chain_image_views();
		virtual void create_frame_buffer_image_view();
		virtual rhi_sampler* get_or_create_default_sampler(render::rhi_default_sampler_type type);
		virtual rhi_sampler* get_or_create_mipmap_sampler(uint32_t width, uint32_t height);
		virtual rhi_shader* create_shader_module(const std::vector<unsigned char>& shader_code);
		virtual void create_buffer(
			render::rhi_device_size size,
			render::rhi_buffer_usage_flags usage, render::rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory, void* data = nullptr, int data_size = 0);
		virtual void copy_buffer(rhi_buffer* src_buffer, rhi_buffer* dst_buffer, render::rhi_device_size src_offset, render::rhi_device_size dst_offset, render::rhi_device_size size);
		virtual void create_image(
			uint32_t image_width, uint32_t image_height, render::rhi_format format,
		                          render::rhi_image_tiling image_tiling,
			render::rhi_image_usage_flags image_usage_flags,
			render::rhi_memory_property_flags memory_property_flags,
			rhi_image*& image,
			rhi_device_memory*& memory,
			render::rhi_image_create_flags image_create_flags,
			uint32_t array_layers,
			uint32_t mip_levels);
		virtual void create_image_view(
			rhi_image* image,
			render::rhi_format format,
			render::rhi_image_aspect_flags image_aspect_flags,
			render::rhi_image_view_type view_type, 
			uint32_t layout_count, uint32_t mip_levels, rhi_image_view*& image_view);
		virtual void create_command_pool();
		virtual bool create_command_pool(const rhi_command_pool_create_info* create_info, rhi_command_pool*& command_pool);
		virtual bool create_descriptor_pool(const rhi_descriptor_pool_create_info* create_info, rhi_descriptor_pool*& descriptor_pool);
		virtual bool create_descriptor_set_layout(const rhi_descriptor_set_layout_create_info* create_info, rhi_descriptor_set_layout*& set_layout);
		virtual bool create_fence(const rhi_fence_create_info* create_info, rhi_fence*& fence);
		virtual bool create_frame_buffer(const rhi_frame_buffer_create_info* create_info, rhi_frame_buffer*& frame_buffer);
		virtual bool create_graphics_pipelines(rhi_pipeline_cache* pipeline_cache, uint32_t create_info_count, const rhi_graphics_pipeline_create_info* create_info, rhi_pipeline*& pipeline);
		virtual bool create_compute_pipelines();
		virtual bool create_pipeline_layout();
		virtual bool create_render_pass();
		virtual bool create_sampler();
		virtual bool create_semaphore();
		virtual std::shared_ptr<rhi_viewport> create_viewport(void* window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen, source_runtime::core::pixel_format pixel_format) = 0;

		// command and command write
		virtual bool wait_for_fences(uint32_t fence_count, rhi_fence* const* fence, render::rhi_bool32 wait_all, uint64_t timeout);
		virtual bool reset_fences(uint32_t fence_count, rhi_fence* const* fence);
		virtual bool reset_command_pool(rhi_command_pool* command_pool, render::rhi_command_pool_reset_flags flags);
		virtual bool begin_command_buffer(rhi_command_buffer* command_buffer, const rhi_command_buffer_begin_info* begin_info);
		virtual bool end_command_buffer(rhi_command_buffer* command_buffer);
		virtual void cmd_begin_render_pass(rhi_command_buffer* command_buffer, const rhi_render_pass_begin_info* render_pass_begin, render::rhi_sub_pass_contents contents);
		virtual void cmd_next_sub_pass(rhi_command_buffer* command_buffer, render::rhi_sub_pass_contents contents);
		virtual void cmd_end_render_pass(rhi_command_buffer* command_buffer);
		virtual void cmd_bind_pipeline(rhi_command_buffer* command_buffer, render::rhi_pipeline_bind_point pipeline_bind_point, rhi_pipeline* pipeline);
		virtual void cmd_set_viewport(rhi_command_buffer* command_buffer, uint32_t first_viewport, uint32_t viewport_count, const rhi_viewport* viewports);
		virtual void cmd_set_scissor(rhi_command_buffer* command_buffer, uint32_t first_scissor, uint32_t scissor_count, const rhi_rect_2d* scissors);
		virtual void cmd_bind_vertex_buffers(rhi_command_buffer* command_buffer, uint32_t first_binding, uint32_t binding_count, rhi_buffer* const* buffers, const render::rhi_device_size* offsets);
		virtual void cmd_bind_index_buffer(rhi_command_buffer* command_buffer, rhi_buffer* buffer, render::rhi_device_size offset, render::rhi_index_type index_type);
		virtual void cmd_bind_descriptor_sets(rhi_command_buffer* command_buffer,
		                                      render::rhi_pipeline_bind_point pipeline_bind_point,
			rhi_pipeline_layout* layout,
			uint32_t first_set,
			uint32_t descriptor_set_count,
			const rhi_descriptor_set* const* descriptors,
			uint32_t dynamic_offset_count,
			const uint32_t* dynamic_offsets);
		virtual void cmd_draw_indexed(rhi_command_buffer* command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance);
		virtual void cmd_clear_attachments(rhi_command_buffer* command_buffer, uint32_t attachment_count, const rhi_clear_attachment* attachment, uint32_t rect_count, const rhi_clear_rect* rects);

		// query
		virtual void get_physical_device_properties(rhi_physical_device_properties* properties);
		virtual [[nodiscard]] rhi_command_buffer* get_current_command_buffer() const;
		virtual rhi_command_buffer* const* get_command_buffer_list() const;
		virtual rhi_command_pool* get_command_pool() const;
		virtual rhi_descriptor_pool* get_descriptor_pool() const;
		virtual rhi_fence* const* get_fence_list() const;
		virtual queue_family_indices get_queue_family_indices() const;
		virtual rhi_queue* get_graphics_queue() const;
		virtual rhi_queue* get_compute_queue() const;
		virtual rhi_swap_chain_desc get_swap_chain_info();
		virtual rhi_depth_image_desc get_depth_image_info();
		virtual uint8_t get_max_frames_in_flight() const;
		virtual uint8_t get_current_frame_index() const;
		virtual void set_current_frame_index(uint8_t index);
		// command write
		virtual rhi_command_buffer* begin_single_time_commands();
		virtual void end_single_time_commands(rhi_command_buffer* command_buffer);
		virtual bool prepare_before_pass(std::function<void()> pass_update_after_recreate_swap_chain);
		virtual void submit_rendering(std::function<void()> pass_update_after_recreate_swap_chain);
		virtual void push_event(rhi_command_buffer* command_buffer, const char* name, const float* color);
		virtual void pop_event(rhi_command_buffer* command_buffer);

		// destroy
		virtual void clear();
		virtual void clear_swap_chain();
		virtual void destroy_default_sampler(render::rhi_default_sampler_type type);
		virtual void destroy_mipmap_sampler();
		virtual void destroy_shader_module(rhi_shader* shader);
		virtual void destroy_semaphore(rhi_semaphore* semaphore);
		virtual void destroy_sampler(rhi_sampler* sampler);
		virtual void destroy_instance(rhi_instance* instance);
		virtual void destroy_image_view(rhi_image_view* image_view);
		virtual void destroy_image(rhi_image* image);
		virtual void destroy_frame_buffer(rhi_frame_buffer* frame_buffer);
		virtual void destroy_fence(rhi_fence* fence);
		virtual void destroy_device();
		virtual void destroy_command_pool(rhi_command_pool* command_pool);
		virtual void destroy_buffer(rhi_buffer*& buffer);
		virtual void free_command_buffer(rhi_command_pool* command_pool, uint32_t command_buffer_count, rhi_command_buffer* command_buffer);
		// memory
		virtual void free_memory(rhi_device_memory*& memory);

		// semaphores
		virtual rhi_semaphore*& get_texture_copy_semaphore(uint32_t index);

		virtual rhi_interface_type get_interface_type() const { return rhi_interface_type::hidden; }
		virtual rhi_compute_context* rhi_get_command_context(rhi_pipeline_type pipeline, rhi_gpu_mask gpu_mask) = 0;

		GLFWwindow* m_window{ nullptr };
		rhi_viewport m_viewport;
	};

	inline rhi_module::~rhi_module() = default;
}
