#include "rhi_module.h"
#include <array>

#include "spdlog/spdlog.h"

namespace source_module::rhi
{
	void rhi_module::initialize(const rhi_init_info init_info)
	{
		
	}

	void rhi_module::prepare_context()
	{
		
	}

	bool rhi_module::is_point_light_shadow_enabled()
	{
		return false;
	}

	bool rhi_module::allocate_command_buffers(const rhi_command_buffer_allocator_info* allocator_info, rhi_command_buffer*& command_buffer)
	{
		return false;
	}

	bool rhi_module::allocate_descriptor_sets(const rhi_descriptor_set_allocator_info* allocator_info, rhi_descriptor_set*& descriptor_set)
	{
		return false;
	}

	void rhi_module::create_swap_chain()
	{
		
	}

	void rhi_module::recreate_swap_chain()
	{
		
	}

	void rhi_module::create_swap_chain_image_views()
	{
		
	}

	void rhi_module::create_frame_buffer_image_view()
	{
		
	}

	rhi_sampler* rhi_module::get_or_create_default_sampler(render::rhi_default_sampler_type type)
	{
		return nullptr;
	}

	rhi_sampler* rhi_module::get_or_create_mipmap_sampler(uint32_t width, uint32_t height)
	{
		return nullptr;
	}

	rhi_shader* rhi_module::create_shader_module(const std::vector<unsigned char>& shader_code)
	{
		return nullptr;
	}

	void rhi_module::create_buffer(render::rhi_device_size size, render::rhi_buffer_usage_flags usage, render::rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory, void* data, int data_size)
	{
		
	}

	void rhi_module::copy_buffer(rhi_buffer* src_buffer, rhi_buffer* dst_buffer, render::rhi_device_size src_offset, render::rhi_device_size dst_offset, render::rhi_device_size size)
	{
		
	}

	void rhi_module::create_image(
		uint32_t image_width, 
		uint32_t image_height, 
		render::rhi_format format, render::rhi_image_tiling image_tiling, render::rhi_image_usage_flags image_usage_flags, render::rhi_memory_property_flags memory_property_flags, rhi_image*& image, rhi_device_memory*& memory, render::rhi_image_create_flags image_create_flags, uint32_t array_layers, uint32_t mip_levels)
	{
		
	}

	void rhi_module::create_image_view(
		rhi_image* image, render::rhi_format format, render::rhi_image_aspect_flags image_aspect_flags, render::rhi_image_view_type view_type, uint32_t layout_count, uint32_t mip_levels, rhi_image_view*& image_view)
	{
		
	}

	void rhi_module::create_command_pool()
	{
		
	}

	bool rhi_module::create_command_pool(const rhi_command_pool_create_info* create_info, rhi_command_pool*& command_pool)
	{
		return false;
	}

	bool rhi_module::create_descriptor_pool(const rhi_descriptor_pool_create_info* create_info, rhi_descriptor_pool*& descriptor_pool)
	{
		return false;
	}

	bool rhi_module::create_descriptor_set_layout(const rhi_descriptor_set_layout_create_info* create_info, rhi_descriptor_set_layout*& set_layout)
	{
		return false;
	}

	bool rhi_module::create_fence(const rhi_fence_create_info* create_info, rhi_fence*& fence)
	{
		return false;
	}

	bool rhi_module::create_frame_buffer(const rhi_frame_buffer_create_info* create_info, rhi_frame_buffer*& frame_buffer)
	{
		return false;
	}

	bool rhi_module::create_graphics_pipelines(rhi_pipeline_cache* pipeline_cache, uint32_t create_info_count, const rhi_graphics_pipeline_create_info* create_info, rhi_pipeline*& pipeline)
	{
		return false;
	}

	bool rhi_module::create_compute_pipelines()
	{
		return false;
	}

	bool rhi_module::create_pipeline_layout()
	{
		return false;
	}

	bool rhi_module::create_render_pass()
	{
		return false;
	}

	bool rhi_module::create_sampler()
	{
		return false;
	}

	bool rhi_module::create_semaphore()
	{
		return false;
	}

	bool rhi_module::wait_for_fences(uint32_t fence_count, rhi_fence* const* fence, render::rhi_bool32 wait_all, uint64_t timeout)
	{
		return false;
	}

	bool rhi_module::reset_fences(uint32_t fence_count, rhi_fence* const* fence)
	{
		return false;
	}

	bool rhi_module::reset_command_pool(rhi_command_pool* command_pool, render::rhi_command_pool_reset_flags flags)
	{
		return false;
	}

	bool rhi_module::begin_command_buffer(rhi_command_buffer* command_buffer, const rhi_command_buffer_begin_info* begin_info)
	{
		return false;
	}

	bool rhi_module::end_command_buffer(rhi_command_buffer* command_buffer)
	{
		return false;
	}

	void rhi_module::cmd_begin_render_pass(rhi_command_buffer* command_buffer, const rhi_render_pass_begin_info* render_pass_begin, render::rhi_sub_pass_contents contents)
	{
		
	}

	void rhi_module::cmd_next_sub_pass(rhi_command_buffer* command_buffer, render::rhi_sub_pass_contents contents)
	{
		
	}

	void rhi_module::cmd_end_render_pass(rhi_command_buffer* command_buffer)
	{
		
	}

	void rhi_module::cmd_bind_pipeline(rhi_command_buffer* command_buffer, render::rhi_pipeline_bind_point pipeline_bind_point, rhi_pipeline* pipeline)
	{
		
	}

	void rhi_module::cmd_set_viewport(rhi_command_buffer* command_buffer, uint32_t first_viewport, uint32_t viewport_count, const rhi_viewport* viewports)
	{
		
	}

	void rhi_module::cmd_set_scissor(rhi_command_buffer* command_buffer, uint32_t first_scissor, uint32_t scissor_count, const rhi_rect_2d* scissors)
	{
		
	}

	void rhi_module::cmd_bind_vertex_buffers(rhi_command_buffer* command_buffer, uint32_t first_binding, uint32_t binding_count, rhi_buffer* const* buffers, const render::rhi_device_size* offsets)
	{
		
	}

	void rhi_module::cmd_bind_index_buffer(rhi_command_buffer* command_buffer, rhi_buffer* buffer, render::rhi_device_size offset, render::rhi_index_type index_type)
	{
		
	}

	void rhi_module::cmd_bind_descriptor_sets(
		rhi_command_buffer* command_buffer, 
		render::rhi_pipeline_bind_point pipeline_bind_point, rhi_pipeline_layout* layout, uint32_t first_set, uint32_t descriptor_set_count, const rhi_descriptor_set* const* descriptors, uint32_t dynamic_offset_count, const uint32_t* dynamic_offsets)
	{
		
	}

	void rhi_module::cmd_draw_indexed(rhi_command_buffer* command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
	{
		
	}

	void rhi_module::cmd_clear_attachments(rhi_command_buffer* command_buffer, uint32_t attachment_count, const rhi_clear_attachment* attachment, uint32_t rect_count, const rhi_clear_rect* rects)
	{
		
	}

	void rhi_module::get_physical_device_properties(rhi_physical_device_properties* properties)
	{
		
	}

	rhi_command_buffer* rhi_module::get_current_command_buffer() const
	{
		return nullptr;
	}

	rhi_command_buffer* const* rhi_module::get_command_buffer_list() const
	{
		return nullptr;
	}

	rhi_command_pool* rhi_module::get_command_pool() const
	{
		return nullptr;
	}

	rhi_descriptor_pool* rhi_module::get_descriptor_pool() const
	{
		return nullptr;
	}

	rhi_fence* const* rhi_module::get_fence_list() const
	{
		return nullptr;
	}

	queue_family_indices rhi_module::get_queue_family_indices() const
	{
		queue_family_indices value;
		return value;
	}

	rhi_queue* rhi_module::get_graphics_queue() const
	{
		return nullptr;
	}

	rhi_queue* rhi_module::get_compute_queue() const
	{
		return nullptr;
	}

	rhi_swap_chain_desc rhi_module::get_swap_chain_info()
	{
		rhi_swap_chain_desc value{};
		return value;
	}

	rhi_depth_image_desc rhi_module::get_depth_image_info()
	{
		rhi_depth_image_desc value{};
		return value;
	}

	uint8_t rhi_module::get_max_frames_in_flight() const
	{
		return 0;
	}

	uint8_t rhi_module::get_current_frame_index() const
	{
		return 0;
	}

	void rhi_module::set_current_frame_index(uint8_t index)
	{
		
	}

	rhi_command_buffer* rhi_module::begin_single_time_commands()
	{
		return nullptr;
	}

	void rhi_module::end_single_time_commands(rhi_command_buffer* command_buffer)
	{
		
	}

	bool rhi_module::prepare_before_pass(std::function<void()> pass_update_after_recreate_swap_chain)
	{
		return false;
	}

	void rhi_module::submit_rendering(std::function<void()> pass_update_after_recreate_swap_chain)
	{
		
	}

	void rhi_module::push_event(rhi_command_buffer* command_buffer, const char* name, const float* color)
	{
		
	}

	void rhi_module::pop_event(rhi_command_buffer* command_buffer)
	{
		
	}

	void rhi_module::clear()
	{
		
	}

	void rhi_module::clear_swap_chain()
	{
		
	}

	void rhi_module::destroy_default_sampler(render::rhi_default_sampler_type type)
	{
		
	}

	void rhi_module::destroy_mipmap_sampler()
	{
		
	}

	void rhi_module::destroy_shader_module(rhi_shader* shader)
	{
		
	}

	void rhi_module::destroy_semaphore(rhi_semaphore* semaphore)
	{
		
	}

	void rhi_module::destroy_sampler(rhi_sampler* sampler)
	{
		
	}

	void rhi_module::destroy_instance(rhi_instance* instance)
	{
		
	}

	void rhi_module::destroy_image_view(rhi_image_view* image_view)
	{
		
	}

	void rhi_module::destroy_image(rhi_image* image)
	{
		
	}

	void rhi_module::destroy_frame_buffer(rhi_frame_buffer* frame_buffer)
	{
		
	}

	void rhi_module::destroy_fence(rhi_fence* fence)
	{
		
	}

	void rhi_module::destroy_device()
	{
		
	}

	void rhi_module::destroy_command_pool(rhi_command_pool* command_pool)
	{
		
	}

	void rhi_module::destroy_buffer(rhi_buffer*& buffer)
	{
		
	}

	void rhi_module::free_command_buffer(rhi_command_pool* command_pool, uint32_t command_buffer_count, rhi_command_buffer* command_buffer)
	{
		
	}

	void rhi_module::free_memory(rhi_device_memory*& memory)
	{
		
	}

	rhi_semaphore*& rhi_module::get_texture_copy_semaphore(uint32_t index)
	{
		SPDLOG_THROW("not implemented.");
	}

}
