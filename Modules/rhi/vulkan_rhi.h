#pragma once

#include "rhi_module.h"
#include "rhi_struct.h"

namespace source_runtime
{
	class vulkan_rhi final : public rhi
	{
	public:
		void initialize(const rhi_init_info init_info) override;
		void create_instance();
		void initialize_debug_messenger();
		void create_window_surface();
		void initialize_physical_device();
		GLFWwindow* m_window{ nullptr };
		VkSurfaceKHR m_surface{ nullptr };
		VkPhysicalDevice m_physical_device{ nullptr };
		rhi_viewport m_viewport;
		rhi_rect_2d m_scissor;
		static uint8_t constexpr k_max_frames_in_flight{ 3 };
	private:
		[[nodiscard]] bool check_validation_layer_support() const;
		std::vector<const char*> get_required_extensions();
		static void populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info);
		static VkResult create_debug_utils_messenger_ext(VkInstance instance,
			const VkDebugUtilsMessengerCreateInfoEXT* create_info,
			const VkAllocationCallbacks* allocator,
			VkDebugUtilsMessengerEXT* debug_utils_messenger);
		bool is_device_suitable(VkPhysicalDevice physical_device);
		swap_chain_support_details query_swap_chain_support(VkPhysicalDevice physical_device) const;
		queue_family_indices find_queue_families(VkPhysicalDevice physical_device) const;
		bool check_device_extension_support(VkPhysicalDevice physical_device);
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
		uint32_t m_vulkan_api_version_{ VK_API_VERSION_1_3 };
		std::vector<char const*> m_device_extensions_{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
		VkInstance m_instance_{ nullptr };

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
	};
}
