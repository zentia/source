#pragma once

#include "Modules/rhi/rhi_module.h"
#include "Modules/rhi/rhi_struct.h"
#include "Modules/rhi/interface/vulkan/vulkan_rhi_resource.h"

namespace source_runtime
{
	class vulkan_rhi final : public rhi
	{
	public:
		void initialize(rhi_init_info init_info) override;
		void prepare_context() override;

		// allocate and create
		bool allocate_command_buffers(const rhi_command_buffer_allocator_info* allocator_info, rhi_command_buffer*& command_buffer) override;
		bool allocate_descriptor_sets(const rhi_descriptor_set_allocator_info* allocator_info, rhi_descriptor_set*& descriptor_sets) override;
		void create_swap_chain() override;
		void recreate_swap_chain() override;
		void create_swap_chain_image_views() override;
		void create_frame_buffer_image_view() override;
		rhi_sampler* get_or_create_default_sampler(rhi_default_sampler_type type) override;
		rhi_sampler* get_or_create_mipmap_sampler(uint32_t width, uint32_t height) override;
		rhi_shader* create_shader_module(const std::vector<unsigned char>& shader_code) override;
		void create_buffer(rhi_device_size size, rhi_buffer_usage_flags usage, rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory) override;
		void create_buffer_and_initialize(rhi_buffer_usage_flags usage, rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory, rhi_device_size size, void* data, int data_size) override;
		bool create_buffer_vma(VmaAllocator allocator, const rhi_buffer_create_info* buffer_create_info, const VmaAllocationCreateInfo* allocation_create_info, rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) override;
		bool create_buffer_with_alignment_vma(VmaAllocator allocator, const rhi_buffer_create_info* buffer_create_info, VmaAllocationCreateInfo* allocation_create_info, rhi_device_size min_alignment, rhi_buffer*& buffer, VmaAllocation* allocation, VmaAllocationInfo* allocation_info) override;
		void copy_buffer(rhi_buffer* src_buffer, rhi_buffer* dst_buffer, rhi_device_size src_offset, rhi_device_size dst_offset, rhi_device_size size) override;
		void create_image(uint32_t image_width, uint32_t image_height, rhi_format format, rhi_image_aspect_flags image_aspect_flags, rhi_image_view view_type, uint32_t layout_count, uint32_t mip_levels, rhi_image_view*& image_view) override;
		
		void create_instance();
		void initialize_debug_messenger();
		void create_window_surface();
		void initialize_physical_device();
		void create_logical_device();

		// destroy
		void destroy_image_view(rhi_image_view* image_view) override;
		queue_family_indices m_queue_family_indices;
		GLFWwindow* m_window{ nullptr };
		VkSurfaceKHR m_surface{ nullptr };
		// 硬件层的抽象
		VkPhysicalDevice m_physical_device{ nullptr };
		// 逻辑设备，逻辑层的抽象
		VkDevice m_device{ nullptr };
		VkSwapchainKHR m_swap_chain{ nullptr };
		std::vector<VkImage> m_swap_chain_images;
		rhi_format m_swap_chain_image_format{ rhi_format_undefined };
		std::vector<rhi_image_view*> m_swap_chain_image_views;
		rhi_extent_2d m_swap_chain_extent;
		rhi_viewport m_viewport;
		rhi_rect_2d m_scissor;
		rhi_format m_depth_image_format{ rhi_format_undefined };
		rhi_image_view* m_depth_image_view = new vulkan_rhi_image_view;
		static uint8_t constexpr k_max_frames_in_flight{ 3 };
		rhi_queue* m_graphics_queue{ nullptr };
		rhi_queue* m_compute_queue{ nullptr };
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
		// asset allocator use VMA library
		VmaAllocator m_asset_allocator_;
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
		VkSemaphore m_image_available_for_texturescopy_semaphores_[k_max_frames_in_flight];
		VkFence m_is_frame_in_flight_fences[k_max_frames_in_flight];

		VkCommandBuffer m_vk_current_command_buffer_;
	};
}
