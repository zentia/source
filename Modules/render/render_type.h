#pragma once

namespace source_runtime
{
#define RHI_MAX_EXTENSION_NAME_SIZE 256u
#define RHI_MAX_DESCRIPTION_SIZE 256u
#define RHI_MAX_MEMORY_TYPES 32u
#define RHI_MAX_PHYSICAL_DEVICE_NAME_SIZE 256u
#define RHI_UUID_SIZE 16u
	enum rhi_structure_type : uint8_t
	{
		rhi_structure_type_application_info = 0,
	};

	enum rhi_command_buffer_level : uint8_t
	{
		rhi_command_buffer_level_primary = 0,
		rhi_command_buffer_level_secondary = 1,
	};

	enum rhi_default_sampler_type : uint8_t
	{
		default_sampler_linear,
		default_sampler_nearest
	};

	enum rhi_shading_mode : uint8_t
	{
		rhi_shading_mode_exclusive = 0,
		rhi_shading_mode_concurrent = 1,
		rhi_shading_mode_max_enum = 0x7f
	};

	enum rhi_format : uint8_t
	{
		rhi_format_undefined = 0,
		rhi_format_r4g4_unorm_pack8 = 1,
		rhi_format_r4g4b4a4_unorm_pack16 = 2,
		rhi_format_b4r4g4r4_unorm_pack16 = 3,

	};

	enum rhi_image_view_type : uint8_t
	{
		rhi_image_view_type_1d = 0,
		rhi_image_view_type_2d = 1,
		rhi_image_view_type_3d = 2,
		rhi_image_view_type_cube = 3,
		rhi_image_view_type_1d_array = 4,
		rhi_image_view_type_2d_array = 5,
		rhi_image_view_type_cube_array = 6,
		rhi_image_view_type_max_enum = 0x7f
	};

	enum rhi_attachment_load_op : uint32_t
	{
		rhi_attachment_load_op_load = 0,
		rhi_attachment_load_op_clear = 1,
		rhi_attachment_load_op_do_not_clear = 2,
		rhi_attachment_load_op_none_ext = 100400000,
		rhi_attachment_load_op_max_enum = 0x7fffffff
	};

	enum rhi_physical_device_type : uint32_t
	{
		rhi_physical_device_type_other = 0,
		rhi_physical_device_type_integrated_gpu = 1,
		rhi_physical_device_type_discrete_gpu = 2,
		rhi_physical_device_type_virtual_gpu = 3,
		rhi_physical_device_type_cpu = 4,
		rhi_physical_device_type_max_enum = 0x7fffffff
	};
	enum rhi_descriptor_type : uint32_t
	{
		rhi_descriptor_type_sampler = 0,
		rhi_descriptor_type_combined_image_sampler = 1,
		rhi_descriptor_type_sampled_image = 2,
		rhi_descriptor_type_storage_image = 3,
		rhi_descriptor_type_uniform_texel_buffer = 4,
		rhi_descriptor_type_storage_texel_buffer = 5,
		rhi_descriptor_type_uniform_buffer = 6,
		rhi_descriptor_type_storage_buffer = 7,
		rhi_descriptor_type_uniform_buffer_dynamic = 8,
		rhi_descriptor_type_storage_buffer_dynamic = 9,
		rhi_descriptor_type_input_attachment = 10,
		rhi_descriptor_type_inline_uniform_block_ext = 100138000,
		rhi_descriptor_type_acceleration_structure_khr = 1000150000,
		rhi_descriptor_type_acceleration_structure_nv = 1000165000,
		rhi_descriptor_type_mutable_value = 1000351000,
		rhi_descriptor_type_max_enum = 0x07fffffff,
	};
	typedef uint32_t rhi_access_flags;
	typedef uint32_t rhi_image_aspect_flags;
	typedef uint32_t rhi_format_feature_flags;
	typedef uint32_t rhi_image_create_flags;
	typedef uint32_t rhi_sample_count_flags;
	typedef uint32_t rhi_image_usage_flags;
	typedef uint32_t rhi_instance_create_flags;
	typedef uint32_t rhi_memory_heap_flags;
	typedef uint32_t rhi_memory_property_flags;
	typedef uint32_t rhi_queue_flags;
	typedef uint32_t rhi_device_create_flags;
	typedef uint32_t rhi_device_queue_create_flags;
	typedef uint32_t rhi_pipeline_stage_flags;
	typedef uint32_t rhi_memory_map_flags;
	typedef uint32_t rhi_sparse_memory_bind_flags;
	typedef uint32_t rhi_sparse_image_format_flags;
	typedef uint32_t rhi_fence_create_flags;
	typedef uint32_t rhi_semaphore_create_flags;
	typedef uint32_t rhi_event_create_flags;
	typedef uint32_t rhi_query_pipeline_statistic_flags;
	typedef uint32_t rhi_query_pool_create_flags;
	typedef uint32_t rhi_query_result_flags;
	typedef uint32_t rhi_buffer_create_flags;
	typedef uint32_t rhi_buffer_usage_flags;
	typedef uint32_t rhi_buffer_view_create_flags;
	typedef uint32_t rhi_shader_module_create_flags;
	typedef uint32_t rhi_pipeline_cache_create_flags;
	typedef uint32_t rhi_color_component_flags;
	typedef uint32_t rhi_pipeline_create_flags;
	typedef uint32_t rhi_pipeline_shader_stage_create_flags;
	typedef uint32_t rhi_cull_mode_flags;
	typedef uint32_t rhi_pipeline_vertex_input_state_create_flags;
	typedef uint32_t rhi_pipeline_input_assembly_state_create_flags;
	typedef uint32_t rhi_pipeline_tessellation_state_create_flags;
	typedef uint32_t rhi_pipeline_viewport_state_create_flags;
	typedef uint32_t rhi_pipeline_rasterization_state_create_flags;
	typedef uint32_t rhi_pipeline_multi_sample_state_create_flags;
	typedef uint32_t rhi_pipeline_depth_stencil_state_create_flags;
	typedef uint32_t rhi_pipeline_color_blend_state_create_flags;
	typedef uint32_t rhi_pipeline_dynamic_create_flags;
	typedef uint32_t rhi_pipeline_layout_create_flags;
	typedef uint32_t rhi_shader_stage_flags;
	typedef uint32_t rhi_sampler_create_flags;
	typedef uint32_t rhi_descriptor_pool_create_flags;
	typedef uint32_t rhi_descriptor_pool_reset_flags;
	typedef uint32_t rhi_descriptor_set_layout_create_flags;
	typedef uint32_t rhi_attachment_description_flags;
	typedef uint32_t rhi_dependency_flags;
	typedef uint32_t rhi_frame_buffer_create_flags;
	typedef uint32_t rhi_render_pass_create_flags;
	typedef uint32_t rhi_command_pool_create_flags;
	typedef uint32_t rhi_sub_pass_description_flags;
	typedef uint32_t rhi_command_pool_reset_flags;
	typedef uint32_t rhi_command_buffer_usage_flags;
	typedef uint32_t rhi_query_control_flags;
	typedef uint32_t rhi_command_buffer_reset_flags;
	typedef uint32_t rhi_stencil_face_flags;
	typedef uint32_t rhi_bool32;
	typedef uint64_t rhi_device_size;
	typedef uint64_t rhi_device_size;
	typedef uint32_t rhi_flags;
	typedef uint32_t rhi_sample_mask;
}
