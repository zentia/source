#pragma once

#include "vulkan_rhi_resource.h"
#include "module/rhi/rhi_module.h"

namespace source_module::rhi
{
	class vulkan_rhi final : public rhi_module
	{
	public:
		vulkan_rhi() = default;
		vulkan_rhi(vulkan_rhi& rhi) = delete;
		vulkan_rhi(vulkan_rhi&& rhi) = delete;
		vulkan_rhi& operator=(vulkan_rhi& rhi) = delete;
		vulkan_rhi& operator=(vulkan_rhi&& rhi) = delete;

		void initialize(rhi_init_info init_info) override;
		void prepare_context() override;

		// allocate and create
		bool allocate_command_buffers(const rhi_command_buffer_allocator_info* allocator_info, rhi_command_buffer*& command_buffer) override;
		bool allocate_descriptor_sets(const rhi_descriptor_set_allocator_info* allocator_info, rhi_descriptor_set*& descriptor_sets) override;
		void create_swap_chain() override;
		void recreate_swap_chain() override;
		void create_swap_chain_image_views() override;
		void create_frame_buffer_image_view() override;
		rhi_sampler* get_or_create_default_sampler(render::rhi_default_sampler_type type) override;
		rhi_sampler* get_or_create_mipmap_sampler(uint32_t width, uint32_t height) override;
		rhi_shader* create_shader_module(const std::vector<unsigned char>& shader_code) override;
		void create_buffer(render::rhi_device_size size, render::rhi_buffer_usage_flags usage, render::rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory, void* data = nullptr, int data_size = 0) override;
		bool create_buffer_vma(VmaAllocator allocator, const rhi_buffer_create_info* rhi_buffer_create_info, const VmaAllocationCreateInfo* allocation_create_info, rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) override;
		bool create_buffer_with_alignment_vma(VmaAllocator allocator,
			const rhi_buffer_create_info* rhi_buffer_create_info, VmaAllocationCreateInfo* allocation_create_info, render::rhi_device_size min_alignment, rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) override;
		void copy_buffer(rhi_buffer* src_buffer, rhi_buffer* dst_buffer, render::rhi_device_size src_offset, render::rhi_device_size dst_offset, render::rhi_device_size size) override;
		void create_image(uint32_t image_width, uint32_t image_height, render::rhi_format format,
		                  render::rhi_image_tiling image_tiling,
		                  render::rhi_image_usage_flags image_usage_flags,
		                  render::rhi_memory_property_flags memory_property_flags,
		                  rhi_image*& image,
		                  rhi_device_memory*& memory,
		                  render::rhi_image_create_flags image_create_flags,
			uint32_t array_layers,
			uint32_t mip_levels) override;
		void create_image_view(rhi_image* image, 
			render::rhi_format format, render::rhi_image_aspect_flags image_aspect_flags, render::rhi_image_view_type view_type, uint32_t layout_count, uint32_t mip_levels, rhi_image_view*& image_view) override;
		void create_global_image(rhi_image*& image,
		                         rhi_image_view*& image_view, 
			VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, void* texture_image_pixels, render::rhi_format texture_image_format, uint32_t mip_levels) override;
		void create_cube_map(rhi_image*& image,
		                     rhi_image_view*& image_view, 
			VmaAllocation& image_allocation, uint32_t texture_image_width, uint32_t texture_image_height, std::array<void*, 6> texture_image_pixels, render::rhi_format texture_image_format, uint32_t mip_levels) override;
		void create_command_pool() override;
		bool create_command_pool(const rhi_command_pool_create_info* create_info, rhi_command_pool*& command_pool) override;
		bool create_descriptor_pool(const rhi_descriptor_pool_create_info* create_info, rhi_descriptor_pool*& descriptor_pool) override;
		bool create_descriptor_set_layout(const rhi_descriptor_set_layout_create_info* create_info, rhi_descriptor_set_layout*& set_layout) override;
		bool create_fence(const rhi_fence_create_info* create_info, rhi_fence*& fence) override;
		bool create_frame_buffer(const rhi_frame_buffer_create_info* create_info, rhi_frame_buffer*& frame_buffer) override;
		bool create_graphics_pipelines(rhi_pipeline_cache* pipeline_cache, uint32_t create_info_count, const rhi_graphics_pipeline_create_info* create_info, rhi_pipeline*& pipeline) override;
		bool create_compute_pipelines() override;
		bool create_pipeline_layout() override;
		bool create_render_pass() override;
		bool create_sampler() override;
		bool create_semaphore() override;
		std::shared_ptr<rhi_viewport> create_viewport(void* window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen, source_runtime::core::pixel_format pixel_format) override;

		// command and command write
		bool wait_for_fences(uint32_t fence_count, rhi_fence* const* fence, render::rhi_bool32 wait_all, uint64_t timeout) override;
		bool reset_fences(uint32_t fence_count, rhi_fence* const* fence) override;
		bool reset_command_pool(rhi_command_pool* command_pool, render::rhi_command_pool_reset_flags flags) override;
		bool begin_command_buffer(rhi_command_buffer* command_buffer, const rhi_command_buffer_begin_info* begin_info) override;
		bool end_command_buffer(rhi_command_buffer* command_buffer) override;
		void cmd_begin_render_pass(rhi_command_buffer* command_buffer, const rhi_render_pass_begin_info* render_pass_begin, render::rhi_sub_pass_contents contents) override;
		void cmd_next_sub_pass(rhi_command_buffer* command_buffer, render::rhi_sub_pass_contents contents) override;
		void cmd_end_render_pass(rhi_command_buffer* command_buffer) override;
		void cmd_bind_pipeline(rhi_command_buffer* command_buffer, render::rhi_pipeline_bind_point pipeline_bind_point, rhi_pipeline* pipeline) override;
		void cmd_set_viewport(rhi_command_buffer* command_buffer, uint32_t first_viewport, uint32_t viewport_count, const rhi_viewport* viewports) override;
		void cmd_set_scissor(rhi_command_buffer* command_buffer, uint32_t first_scissor, uint32_t scissor_count, const rhi_rect_2d* scissors) override;
		void cmd_bind_vertex_buffers(rhi_command_buffer* command_buffer, uint32_t first_binding, uint32_t binding_count, rhi_buffer* const* buffers, const render::rhi_device_size* offsets) override;
		void cmd_bind_index_buffer(rhi_command_buffer* command_buffer, rhi_buffer* buffer, render::rhi_device_size offset, render::rhi_index_type index_type) override;
		void cmd_bind_descriptor_sets(rhi_command_buffer* command_buffer, render::rhi_pipeline_bind_point pipeline_bind_point, rhi_pipeline_layout* layout, uint32_t first_set, uint32_t descriptor_set_count, const rhi_descriptor_set* const* descriptors, uint32_t dynamic_offset_count, const uint32_t* dynamic_offsets) override;
		void cmd_draw_indexed(rhi_command_buffer* command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance) override;
		void cmd_clear_attachments(rhi_command_buffer* command_buffer, uint32_t attachment_count, const rhi_clear_attachment* attachment, uint32_t rect_count, const rhi_clear_rect* rects) override;

		// query
		void get_physical_device_properties(rhi_physical_device_properties* properties) override;
		rhi_command_buffer* get_current_command_buffer() const override;
		rhi_command_buffer* const* get_command_buffer_list() const override;
		rhi_command_pool* get_command_pool() const override;
		rhi_descriptor_pool* get_descriptor_pool() const override;
		rhi_fence* const* get_fence_list() const override;
		queue_family_indices get_queue_family_indices() const override;
		rhi_queue* get_graphics_queue() const override;
		rhi_queue* get_compute_queue() const override;
		rhi_swap_chain_desc get_swap_chain_info() override;
		rhi_depth_image_desc get_depth_image_info() override;
		uint8_t get_max_frames_in_flight() const override;
		uint8_t get_current_frame_index() const override;
		void set_current_frame_index(uint8_t index) override;

		// command write
		rhi_command_buffer* begin_single_time_commands() override;
		void end_single_time_commands(rhi_command_buffer* command_buffer) override;
		bool prepare_before_pass(std::function<void()> pass_update_after_recreate_swap_chain) override;
		void submit_rendering(std::function<void()> pass_update_after_recreate_swap_chain) override;
		void push_event(rhi_command_buffer* command_buffer, const char* name, const float* color) override;
		void pop_event(rhi_command_buffer* command_buffer) override;

		// destroy
		virtual ~vulkan_rhi() override final;
		void clear() override;
		void clear_swap_chain() override;
		void destroy_default_sampler(render::rhi_default_sampler_type type) override;
		void destroy_mipmap_sampler() override;
		void destroy_shader_module(rhi_shader* shader) override;
		void destroy_semaphore(rhi_semaphore* semaphore) override;
		void destroy_sampler(rhi_sampler* sampler) override;
		void destroy_instance(rhi_instance* instance) override;
		void destroy_image_view(rhi_image_view* image_view) override;
		void destroy_image(rhi_image* image) override;
		void destroy_frame_buffer(rhi_frame_buffer* frame_buffer) override;
		void destroy_fence(rhi_fence* fence) override;
		void destroy_device() override;
		void destroy_command_pool(rhi_command_pool* command_pool) override;
		void destroy_buffer(rhi_buffer*& buffer) override;
		void free_command_buffer(rhi_command_pool* command_pool, uint32_t command_buffer_count, rhi_command_buffer* command_buffer) override;
		// memory
		void free_memory(rhi_device_memory*& memory) override;

		// semaphores
		rhi_semaphore*& get_texture_copy_semaphore(uint32_t index) override;

		bool is_point_light_shadow_enabled() override;
		rhi_interface_type get_interface_type() const override;
		rhi_compute_context* rhi_get_command_context(rhi_pipeline_type pipeline, rhi_gpu_mask gpu_mask) override;
		void create_instance();
		void initialize_debug_messenger();
		void create_window_surface();
		void initialize_physical_device();
		void create_logical_device();

		queue_family_indices m_queue_family_indices;
		
		VkSurfaceKHR m_surface{ nullptr };
		// 硬件层的抽象
		VkPhysicalDevice m_physical_device{ nullptr };
		// 逻辑设备，逻辑层的抽象
		VkDevice m_device{ nullptr };
		VkSwapchainKHR m_swap_chain{ nullptr };
		std::vector<VkImage> m_swap_chain_images;
		render::rhi_format m_swap_chain_image_format{render::rhi_format_undefined };
		std::vector<rhi_image_view*> m_swap_chain_image_views;
		rhi_extent_2d m_swap_chain_extent;
		
		rhi_rect_2d m_scissor;
		render::rhi_format m_depth_image_format{render::rhi_format_undefined };
		rhi_image_view* m_depth_image_view = new vulkan_rhi_image_view;
		static uint8_t constexpr k_max_frames_in_flight{ 3 };
		rhi_queue* m_graphics_queue{ nullptr };
		rhi_queue* m_compute_queue{ nullptr };
		// asset allocator use VMA library
		VmaAllocator m_asset_allocator;
		VkCommandBuffer m_vk_command_buffer;
	private:
		[[nodiscard]] bool check_validation_layer_support() const;
		std::vector<const char*> get_required_extensions();
		static void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
		static VkResult create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_utils_messenger);
		bool is_device_suitable(VkPhysicalDevice physical_device);
		swap_chain_support_details query_swap_chain_support(VkPhysicalDevice physical_device) const;
		queue_family_indices find_queue_families(VkPhysicalDevice physical_device) const;
		bool check_device_extension_support(VkPhysicalDevice physical_device);
		static VkSurfaceFormatKHR choose_swap_chain_surface_format_form_details(const std::vector<VkSurfaceFormatKHR>& available_surface_formats);
		static VkPresentModeKHR choose_swap_chain_present_mode_from_details(const std::vector<VkPresentModeKHR>& available_present_modes);
		VkExtent2D choose_swap_chain_extent_from_details(const VkSurfaceCapabilitiesKHR& capabilities) const;
		VkDebugUtilsMessengerEXT m_debug_messenger_ = nullptr;
		bool m_enable_validation_layers_{ true };
		bool m_enable_debug_utils_label_{ true };
		bool m_enable_point_light_shadow_{ true };
		// 检查Vulkan API的调用是否符合规范，确保正确的参数和调用顺序
		// 捕获常见的错误，如未正确同步命令，资源泄露，未使用的资源
		// 检查内存分配和释放的正确性，确保没有内存泄露和未定义的行为
		// 提供性能优化的建议
		// 输出详细的调试星系
		const std::vector<char const*> m_validation_layers_{ "VK_LAYER_KHRONOS_validation" };
		uint32_t m_vulkan_api_version_{ VK_API_VERSION_1_0 };
		std::vector<char const*> m_device_extensions_{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };

		// default sampler cache
		rhi_sampler* m_linear_sampler_ = nullptr;
		rhi_sampler* m_nearest_sampler_ = nullptr;
		std::map<uint32_t, rhi_sampler*> m_mipmap_sampler_map_;
		VkInstance m_instance_{ nullptr };

		rhi_command_buffer* m_current_command_buffer_ = new vulkan_rhi_command_buffer();

		rhi_image* m_depth_image_ = new vulkan_rhi_image();
		VkDeviceMemory m_depth_image_memory_{ nullptr };
		std::vector<VkFramebuffer> m_swap_chain_frame_buffers_;
		
		// function pointers
		PFN_vkCmdBeginDebugUtilsLabelEXT m_vk_cmd_begin_debug_utils_label_ext_;
		PFN_vkCmdEndDebugUtilsLabelEXT m_vk_cmd_end_debug_utils_label_ext_;
		PFN_vkWaitForFences m_vk_wait_for_fences_;
		PFN_vkResetFences m_vk_reset_fences_;
		PFN_vkResetCommandPool m_vk_reset_command_pool_;
		PFN_vkResetCommandBuffer m_vk_reset_command_buffer_;
		PFN_vkBeginCommandBuffer m_vk_begin_command_buffer_;
		PFN_vkEndCommandBuffer m_vk_end_command_buffer_;
		PFN_vkCmdBeginRenderPass2 m_vk_cmd_begin_render_pass2_;
		PFN_vkCmdNextSubpass2 m_vk_cmd_next_subpass2_;
		PFN_vkCmdEndRenderPass2 m_vk_cmd_end_render_pass2_;
		PFN_vkCmdBindPipeline m_vk_cmd_bind_pipeline_;
		PFN_vkCmdSetViewport m_vk_cmd_set_viewport_;
		// 用于设置当前命令缓存区的裁剪矩形(scissor rectangle)，裁剪矩形定义了在视口那些像素会被实际绘制，只有在这个矩形内的像素才会被绘制。
		PFN_vkCmdSetScissor m_vk_cmd_set_scissor_;
		PFN_vkCmdBindVertexBuffers2 m_vk_cmd_bind_vertex_buffers2_;
		PFN_vkCmdBindIndexBuffer m_vk_cmd_bind_index_buffer_;
		PFN_vkCmdBindDescriptorSets m_vk_cmd_bind_descriptor_sets_;
		PFN_vkCmdDrawIndexed m_vk_cmd_draw_indexed_;
		PFN_vkCmdClearAttachments m_vk_cmd_clear_attachments_;

		// global descriptor pool
		VkDescriptorPool m_vk_descriptor_pool_;

		// command pool and buffers
		uint8_t m_current_frame_index_{ 0 };
		VkCommandPool m_command_pools_[k_max_frames_in_flight];
		VkCommandBuffer m_vk_command_buffers_[k_max_frames_in_flight];
		VkSemaphore m_image_available_for_render_semaphores_[k_max_frames_in_flight];
		VkSemaphore m_image_finished_for_presentation_semaphores_[k_max_frames_in_flight];
		rhi_semaphore* m_image_available_for_texturescopy_semaphores_[k_max_frames_in_flight];
		VkFence m_is_frame_in_flight_fences[k_max_frames_in_flight];

		VkCommandBuffer m_vk_current_command_buffer_;
	};
}
