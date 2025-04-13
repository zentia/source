#include "vulkan_rhi.h"

#include <iostream>

#include "module/rhi/rhi_module.h"
#include "spdlog/spdlog.h"
#include <algorithm>
#include <filesystem>

#include "vulkan_util.h"

namespace source_module::rhi
{
	void vulkan_rhi::initialize(const rhi_init_info init_info)
	{
		m_window = init_info.window_module->get_window();

		glm::vec2 window_size = init_info.window_module->get_window_size();
		const float width = static_cast<float>(window_size[0]);
		const float height = static_cast<float>(window_size[1]);
		m_viewport = { 0.0f,0.0f, width, height, 0.0f, 1.0f };
		m_scissor = { {0,0},	{(uint32_t)(window_size.x), (uint32_t)(window_size[1])} };
#ifdef DEBUG
		m_enable_validation_layers_ = true;
		m_enable_debug_utils_label_ = true;
#else
		m_enable_debug_utils_label_ = false;
		m_enable_debug_utils_label_ = false;
#endif

		create_instance();
		initialize_debug_messenger();
		create_window_surface();
		initialize_physical_device();
		create_logical_device();
	}

	void vulkan_rhi::prepare_context()
	{
		m_vk_current_command_buffer_ = m_vk_command_buffers_[m_current_frame_index_];
		static_cast<vulkan_rhi_command_buffer*>(m_current_command_buffer_)->set_resource(m_vk_current_command_buffer_);
	}

	bool vulkan_rhi::allocate_command_buffers(const rhi_command_buffer_allocator_info* allocator_info, rhi_command_buffer*& command_buffer)
	{
		const VkCommandBufferAllocateInfo command_buffer_allocate_info
		{
			.sType = static_cast<VkStructureType>(allocator_info->type),
			.pNext = static_cast<const void*>(allocator_info->next),
			.commandPool = static_cast<vulkan_rhi_command_pool*>(allocator_info->command_pool)->get_resource(),
			.level = static_cast<VkCommandBufferLevel>(allocator_info->level),
			.commandBufferCount = allocator_info->command_buffer_count
		};

		VkCommandBuffer vk_command_buffer;
		command_buffer = new rhi_command_buffer;
		const VkResult result = vkAllocateCommandBuffers(m_device, &command_buffer_allocate_info, &vk_command_buffer);
		static_cast<vulkan_rhi_command_buffer*>(command_buffer)->set_resource(vk_command_buffer);

		if (result == VK_SUCCESS)
		{
			return true;
		}
		SPDLOG_ERROR("vkAllocateCommandBuffers failed!");
		return false;
	}

	bool vulkan_rhi::allocate_descriptor_sets(const rhi_descriptor_set_allocator_info* allocator_info, rhi_descriptor_set*& descriptor_sets)
	{
		const uint32_t descriptor_set_layout_size = allocator_info->descriptor_set_count;
		std::vector<VkDescriptorSetLayout> vk_descriptor_set_layout_list(descriptor_set_layout_size);
		for (int i = 0; i < descriptor_set_layout_size; ++i)
		{
			const auto& rhi_descriptor_set_layout_element = allocator_info->set_layouts[i];
			auto& vk_descriptor_set_layout_element = vk_descriptor_set_layout_list[i];

			vk_descriptor_set_layout_element = static_cast<vulkan_rhi_descriptor_set_layout*>(rhi_descriptor_set_layout_element)->get_resource();
		}

		VkDescriptorSetAllocateInfo descriptor_set_allocate_info
		{
			.sType = static_cast<VkStructureType>(allocator_info->type),
			.pNext = static_cast<const void*>(allocator_info->next),
			.descriptorPool = static_cast<vulkan_rhi_descriptor_pool*>(allocator_info->descriptor_pool)->get_resource(),
			.descriptorSetCount = allocator_info->descriptor_set_count,
			.pSetLayouts = vk_descriptor_set_layout_list.data()
		};

		VkDescriptorSet vk_descriptor_set;
		descriptor_sets = new vulkan_rhi_descriptor_set;
		const VkResult result = vkAllocateDescriptorSets(m_device, &descriptor_set_allocate_info, &vk_descriptor_set);
		static_cast<vulkan_rhi_descriptor_set*>(descriptor_sets)->set_resource(vk_descriptor_set);

		if (result == VK_SUCCESS)
		{
			return true;
		}
		SPDLOG_ERROR("vkAllocateDescriptorSets failed!");
		return false;
	}

	void vulkan_rhi::create_swap_chain()
	{
		const swap_chain_support_details swap_chain_support_details = query_swap_chain_support(m_physical_device);

		VkSurfaceFormatKHR surface_format = choose_swap_chain_surface_format_form_details(swap_chain_support_details.format);
		VkPresentModeKHR present_mode = choose_swap_chain_present_mode_from_details(swap_chain_support_details.present_mode);
		const VkExtent2D extent_2d = choose_swap_chain_extent_from_details(swap_chain_support_details.capabilities);

		uint32_t image_count = swap_chain_support_details.capabilities.minImageCount + 1;
		if (swap_chain_support_details.capabilities.minImageCount > 0 && image_count > swap_chain_support_details.capabilities.maxImageCount)
		{
			image_count = swap_chain_support_details.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info
		{
			.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
			.surface = m_surface,
			.minImageCount = image_count,
			.imageFormat = surface_format.format,
			.imageColorSpace = surface_format.colorSpace,
			.imageExtent = extent_2d,
			.imageArrayLayers = 1,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT
		};

		uint32_t queue_family_indices[] = { m_queue_family_indices.graphics_family.value(), m_queue_family_indices.present_family.value() };

		if (m_queue_family_indices.graphics_family != m_queue_family_indices.present_family)
		{
			// 指定图像可以用于颜色附件
			create_info.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			create_info.queueFamilyIndexCount = 2;
			create_info.pQueueFamilyIndices = queue_family_indices;
		}
		else
		{
			// 任何资源只能被一个队列使用，可以提供性能，因为它避免了多队列访问同一资源时可能出现的同步问题。这种模式通常会导致更见的资源管理和更高的效率
			create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		create_info.preTransform = swap_chain_support_details.capabilities.currentTransform;
		// 合成时完全不透明
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.presentMode = present_mode;
		create_info.clipped = VK_TRUE;

		create_info.oldSwapchain = VK_NULL_HANDLE;

		if (vkCreateSwapchainKHR(m_device, &create_info, nullptr, &m_swap_chain) != VK_SUCCESS)
		{
			SPDLOG_ERROR("vk create swap chain khr");
		}

		vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, nullptr);
		m_swap_chain_images.resize(image_count);
		vkGetSwapchainImagesKHR(m_device, m_swap_chain, &image_count, m_swap_chain_images.data());

		m_swap_chain_image_format = static_cast<render::rhi_format>(surface_format.format);
		m_swap_chain_extent.height = extent_2d.height;
		m_swap_chain_extent.width = extent_2d.width;

		m_scissor = { {0,0},{m_swap_chain_extent.width,m_swap_chain_extent.height} };
	}

	void vulkan_rhi::recreate_swap_chain()
	{
		int width = 0;
		int height = 0;
		glfwGetFramebufferSize(m_window, &width, &height);
		while (width == 0 || height == 0) // minimized 0,0, pause for now
		{
			glfwGetFramebufferSize(m_window, &width, &height);
			glfwWaitEvents();
		}

		VkResult res_wait_for_fences = m_vk_wait_for_fences_(m_device, k_max_frames_in_flight, m_is_frame_in_flight_fences, VK_TRUE, UINT64_MAX);
		if (VK_SUCCESS != res_wait_for_fences)
		{
			SPDLOG_ERROR("m_vk_wait_for_fences_ failed");
			return;
		}

		destroy_image_view(m_depth_image_view);
		vkDestroyImage(m_device, static_cast<vulkan_rhi_image*>(m_depth_image_)->get_resource(), nullptr);
		vkFreeMemory(m_device, m_depth_image_memory_, nullptr);

		for (const auto image_view : m_swap_chain_image_views)
		{
			vkDestroyImageView(m_device, static_cast<vulkan_rhi_image_view*>(image_view)->get_resource(), nullptr);
		}

		create_swap_chain();
		create_swap_chain_image_views();
		create_frame_buffer_image_view();
	}

	void vulkan_rhi::create_swap_chain_image_views()
	{
		m_swap_chain_image_views.resize(m_swap_chain_images.size());

		for (size_t i = 0; i < m_swap_chain_images.size(); ++i)
		{
			const VkImageView vk_image_view = source_runtime::vulkan_util::create_image_view(m_device, m_swap_chain_images[i], static_cast<VkFormat>(m_swap_chain_image_format), VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_VIEW_TYPE_2D, 1, 1);
			m_swap_chain_image_views[i] = new vulkan_rhi_image_view;
			static_cast<vulkan_rhi_image_view*>(m_swap_chain_image_views[i])->set_resource(vk_image_view);
		}
	}

	void vulkan_rhi::create_frame_buffer_image_view()
	{
		source_runtime::vulkan_util::create_image(
			m_physical_device,
			m_device,
			m_swap_chain_extent.width,
			m_swap_chain_extent.height,
			static_cast<VkFormat>(m_depth_image_format),
			VK_IMAGE_TILING_OPTIMAL,
			VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
			static_cast<vulkan_rhi_image*>(m_depth_image_)->get_resource(),
			m_depth_image_memory_,
			0,
			1,
			1
		);

		static_cast<vulkan_rhi_image_view*>(m_depth_image_view)->set_resource(source_runtime::vulkan_util::create_image_view(
			m_device,
			static_cast<vulkan_rhi_image*>(m_depth_image_)->get_resource(),
			static_cast<VkFormat>(m_depth_image_format),
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VK_IMAGE_VIEW_TYPE_2D,
			1,
			1
		));
	}

	rhi_sampler* vulkan_rhi::get_or_create_default_sampler(render::rhi_default_sampler_type type)
	{
		switch (type)
		{
		case render::default_sampler_linear:
			if (m_linear_sampler_ == nullptr)
			{
				m_linear_sampler_ = new vulkan_rhi_sampler;
				static_cast<vulkan_rhi_sampler*>(m_linear_sampler_)->set_resource(source_runtime::vulkan_util::get_or_create_linear_sampler(m_physical_device, m_device));
			}
			return m_linear_sampler_;
		case render::default_sampler_nearest:
			if (m_nearest_sampler_ == nullptr)
			{
				m_nearest_sampler_ = new vulkan_rhi_sampler;
				static_cast<vulkan_rhi_sampler*>(m_nearest_sampler_)->set_resource(source_runtime::vulkan_util::get_or_create_nearest_sampler(m_physical_device, m_device));
			}
			return m_nearest_sampler_;
		default:
			return nullptr;
		}
	}

	rhi_sampler* vulkan_rhi::get_or_create_mipmap_sampler(uint32_t width, uint32_t height)
	{
		if (width == 0 || height == 0)
		{
			SPDLOG_ERROR("width == 0 || height == 0");
			return nullptr;
		}

		uint32_t mip_levels = floor(log2((std::max)(width, height))) + 1;
		auto find_sampler = m_mipmap_sampler_map_.find(mip_levels);
		if (find_sampler != m_mipmap_sampler_map_.end())
		{
			return find_sampler->second;
		}
		rhi_sampler* sampler = new vulkan_rhi_sampler;

		VkSampler vk_sampler = source_runtime::vulkan_util::get_or_create_mipmap_sampler(m_physical_device, m_device, width, height);

		static_cast<vulkan_rhi_sampler*>(sampler)->set_resource(vk_sampler);

		m_mipmap_sampler_map_.insert(std::make_pair(mip_levels, sampler));

		return sampler;
	}

	rhi_shader* vulkan_rhi::create_shader_module(const std::vector<unsigned char>& shader_code)
	{
		rhi_shader* shader = new vulkan_rhi_shader;

		VkShaderModule vk_shader = source_runtime::vulkan_util::create_shader_module(m_device, shader_code);

		static_cast<vulkan_rhi_shader*>(shader)->set_resource(vk_shader);

		return shader;
	}

	void vulkan_rhi::create_buffer(render::rhi_device_size size, render::rhi_buffer_usage_flags usage, render::rhi_memory_property_flags properties, rhi_buffer*& buffer, rhi_device_memory*& buffer_memory, void* data, int data_size)
	{
		VkBuffer vk_buffer;
		VkDeviceMemory vk_device_memory;

		source_runtime::vulkan_util::create_buffer_and_initialize(m_device, m_physical_device, usage, properties, &vk_buffer, &vk_device_memory, size);

		buffer = new vulkan_rhi_buffer;
		buffer_memory = new vulkan_rhi_device_memory;
		static_cast<vulkan_rhi_buffer*>(buffer)->set_resource(vk_buffer);
		static_cast<vulkan_rhi_device_memory*>(buffer_memory)->set_resource(vk_device_memory);
	}

	void vulkan_rhi::copy_buffer(rhi_buffer* src_buffer, rhi_buffer* dst_buffer, const render::rhi_device_size src_offset, const render::rhi_device_size dst_offset, const render::rhi_device_size size)
	{
		const VkBuffer vk_src_buffer = static_cast<vulkan_rhi_buffer*>(src_buffer)->get_resource();
		const VkBuffer vk_dst_buffer = static_cast<vulkan_rhi_buffer*>(dst_buffer)->get_resource();
		source_runtime::vulkan_util::copy_buffer(this, vk_src_buffer, vk_dst_buffer, src_offset, dst_offset, size);
	}

	void vulkan_rhi::create_image(const uint32_t image_width,
		const uint32_t image_height, render::rhi_format format,
		render::rhi_image_tiling image_tiling,
		const render::rhi_image_usage_flags image_usage_flags,
		const render::rhi_memory_property_flags memory_property_flags,
		rhi_image*& image,
		rhi_device_memory*& memory,
		const render::rhi_image_create_flags image_create_flags,
		const uint32_t array_layers,
		const uint32_t mip_levels)
	{
		VkImage vk_image;
		VkDeviceMemory vk_device_memory;
		source_runtime::vulkan_util::create_image(m_physical_device, m_device, image_width, image_height, static_cast<VkFormat>(format), static_cast<VkImageTiling>(image_tiling), image_usage_flags, memory_property_flags, vk_image, vk_device_memory,
			image_create_flags, array_layers, mip_levels);

		image = new vulkan_rhi_image;
		memory = new vulkan_rhi_device_memory;
		static_cast<vulkan_rhi_image*>(image)->set_resource(vk_image);
		static_cast<vulkan_rhi_device_memory*>(memory)->set_resource(vk_device_memory);
	}

	void vulkan_rhi::create_image_view(
		rhi_image* image, render::rhi_format format, render::rhi_image_aspect_flags image_aspect_flags, render::rhi_image_view_type view_type, uint32_t layout_count, uint32_t mip_levels,
		rhi_image_view*& image_view
	)
	{
		image_view = new rhi_image_view;
		VkImage vk_image = static_cast<vulkan_rhi_image*>(image)->get_resource();
		const VkImageView vk_image_view = source_runtime::vulkan_util::create_image_view(m_device, vk_image, static_cast<VkFormat>(format), image_aspect_flags, static_cast<VkImageViewType>(view_type), layout_count, mip_levels);
		static_cast<vulkan_rhi_image_view*>(image_view)->set_resource(vk_image_view);
	}

	void vulkan_rhi::create_command_pool()
	{

	}

	bool vulkan_rhi::create_command_pool(const rhi_command_pool_create_info* create_info, rhi_command_pool*& command_pool)
	{
		return false;
	}

	bool vulkan_rhi::create_descriptor_pool(const rhi_descriptor_pool_create_info* create_info, rhi_descriptor_pool*& descriptor_pool)
	{
		return false;
	}

	bool vulkan_rhi::create_descriptor_set_layout(const rhi_descriptor_set_layout_create_info* create_info, rhi_descriptor_set_layout*& set_layout)
	{
		return false;
	}

	bool vulkan_rhi::create_fence(const rhi_fence_create_info* create_info, rhi_fence*& fence)
	{
		return false;
	}

	bool vulkan_rhi::create_frame_buffer(const rhi_frame_buffer_create_info* create_info, rhi_frame_buffer*& frame_buffer)
	{
		return false;
	}

	bool vulkan_rhi::create_graphics_pipelines(rhi_pipeline_cache* pipeline_cache, uint32_t create_info_count, const rhi_graphics_pipeline_create_info* create_info, rhi_pipeline*& pipeline)
	{
		return false;
	}

	bool vulkan_rhi::create_compute_pipelines()
	{
		return false;
	}

	bool vulkan_rhi::create_pipeline_layout()
	{
		return false;
	}

	bool vulkan_rhi::create_render_pass()
	{
		return false;
	}

	bool vulkan_rhi::create_sampler()
	{
		return false;
	}

	bool vulkan_rhi::create_semaphore()
	{
		return false;
	}

	std::shared_ptr<rhi_viewport> vulkan_rhi::create_viewport(void* window_handle, uint32_t size_x, uint32_t size_y, bool is_fullscreen, source_runtime::core::pixel_format pixel_format)
	{
		return nullptr;
	}

	bool vulkan_rhi::wait_for_fences(uint32_t fence_count, rhi_fence* const* fence, render::rhi_bool32 wait_all, uint64_t timeout)
	{
		return false;
	}

	bool vulkan_rhi::reset_fences(uint32_t fence_count, rhi_fence* const* fence)
	{
		return false;
	}

	bool vulkan_rhi::reset_command_pool(rhi_command_pool* command_pool, render::rhi_command_pool_reset_flags flags)
	{
		return false;
	}

	bool vulkan_rhi::begin_command_buffer(rhi_command_buffer* command_buffer, const rhi_command_buffer_begin_info* begin_info)
	{
		return false;
	}

	bool vulkan_rhi::end_command_buffer(rhi_command_buffer* command_buffer)
	{
		return false;
	}

	void vulkan_rhi::cmd_begin_render_pass(rhi_command_buffer* command_buffer, const rhi_render_pass_begin_info* render_pass_begin, render::rhi_sub_pass_contents contents)
	{
		return;
	}

	void vulkan_rhi::cmd_next_sub_pass(rhi_command_buffer* command_buffer, render::rhi_sub_pass_contents contents)
	{

	}

	void vulkan_rhi::cmd_end_render_pass(rhi_command_buffer* command_buffer)
	{

	}

	void vulkan_rhi::cmd_bind_pipeline(rhi_command_buffer* command_buffer, render::rhi_pipeline_bind_point pipeline_bind_point, rhi_pipeline* pipeline)
	{

	}

	void vulkan_rhi::cmd_set_viewport(rhi_command_buffer* command_buffer, uint32_t first_viewport, uint32_t viewport_count, const rhi_viewport* viewports)
	{

	}

	void vulkan_rhi::cmd_set_scissor(rhi_command_buffer* command_buffer, uint32_t first_scissor, uint32_t scissor_count, const rhi_rect_2d* scissors)
	{

	}

	void vulkan_rhi::cmd_bind_vertex_buffers(rhi_command_buffer* command_buffer, uint32_t first_binding, uint32_t binding_count, rhi_buffer* const* buffers, const render::rhi_device_size* offsets)
	{

	}

	void vulkan_rhi::cmd_bind_index_buffer(rhi_command_buffer* command_buffer, rhi_buffer* buffer, render::rhi_device_size offset, render::rhi_index_type index_type)
	{

	}

	void vulkan_rhi::cmd_bind_descriptor_sets(
		rhi_command_buffer* command_buffer,
		render::rhi_pipeline_bind_point pipeline_bind_point,
		rhi_pipeline_layout* layout, uint32_t first_set, uint32_t descriptor_set_count, const rhi_descriptor_set* const* descriptors, uint32_t dynamic_offset_count, const uint32_t* dynamic_offsets)
	{

	}

	void vulkan_rhi::cmd_draw_indexed(rhi_command_buffer* command_buffer, uint32_t index_count, uint32_t instance_count, uint32_t first_index, int32_t vertex_offset, uint32_t first_instance)
	{

	}

	void vulkan_rhi::cmd_clear_attachments(rhi_command_buffer* command_buffer, uint32_t attachment_count, const rhi_clear_attachment* attachment, uint32_t rect_count, const rhi_clear_rect* rects)
	{

	}

	void vulkan_rhi::get_physical_device_properties(rhi_physical_device_properties* properties)
	{

	}

	rhi_command_buffer* vulkan_rhi::get_current_command_buffer() const
	{
		return nullptr;
	}

	rhi_command_buffer* const* vulkan_rhi::get_command_buffer_list() const
	{
		return nullptr;
	}

	rhi_command_pool* vulkan_rhi::get_command_pool() const
	{
		return nullptr;
	}

	rhi_descriptor_pool* vulkan_rhi::get_descriptor_pool() const
	{
		return nullptr;
	}

	rhi_fence* const* vulkan_rhi::get_fence_list() const
	{
		return nullptr;
	}

	queue_family_indices vulkan_rhi::get_queue_family_indices() const
	{
		return m_queue_family_indices;
	}

	rhi_queue* vulkan_rhi::get_graphics_queue() const
	{
		return nullptr;
	}

	rhi_queue* vulkan_rhi::get_compute_queue() const
	{
		return nullptr;
	}

	rhi_swap_chain_desc vulkan_rhi::get_swap_chain_info()
	{
		rhi_swap_chain_desc desc;
		desc.image_format = m_swap_chain_image_format;
		desc.extent = m_swap_chain_extent;
		desc.viewport = &m_viewport;
		desc.scissor = &m_scissor;
		desc.image_views = m_swap_chain_image_views;
		return desc;
	}

	rhi_depth_image_desc vulkan_rhi::get_depth_image_info()
	{
		rhi_depth_image_desc desc;
		desc.depth_image_format = m_depth_image_format;
		desc.depth_image_view = m_depth_image_view;
		desc.depth_image = m_depth_image_;
		return desc;
	}

	uint8_t vulkan_rhi::get_max_frames_in_flight() const
	{
		return 0;
	}

	uint8_t vulkan_rhi::get_current_frame_index() const
	{
		return 0;
	}

	void vulkan_rhi::set_current_frame_index(uint8_t index)
	{

	}

	rhi_command_buffer* vulkan_rhi::begin_single_time_commands()
	{
		return nullptr;
	}

	void vulkan_rhi::end_single_time_commands(rhi_command_buffer* command_buffer)
	{

	}

	bool vulkan_rhi::prepare_before_pass(std::function<void()> pass_update_after_recreate_swap_chain)
	{
		return false;
	}

	void vulkan_rhi::submit_rendering(std::function<void()> pass_update_after_recreate_swap_chain)
	{

	}

	void vulkan_rhi::push_event(rhi_command_buffer* command_buffer, const char* name, const float* color)
	{

	}

	void vulkan_rhi::pop_event(rhi_command_buffer* command_buffer)
	{

	}

	void vulkan_rhi::clear()
	{

	}

	vulkan_rhi::~vulkan_rhi()
	{

	}

	void vulkan_rhi::clear_swap_chain()
	{

	}

	void vulkan_rhi::destroy_default_sampler(render::rhi_default_sampler_type type)
	{

	}

	void vulkan_rhi::destroy_mipmap_sampler()
	{

	}

	void vulkan_rhi::destroy_shader_module(rhi_shader* shader)
	{

	}

	void vulkan_rhi::destroy_semaphore(rhi_semaphore* semaphore)
	{

	}

	void vulkan_rhi::destroy_sampler(rhi_sampler* sampler)
	{

	}

	void vulkan_rhi::destroy_instance(rhi_instance* instance)
	{

	}

	void vulkan_rhi::destroy_image(rhi_image* image)
	{

	}

	void vulkan_rhi::destroy_frame_buffer(rhi_frame_buffer* frame_buffer)
	{

	}

	void vulkan_rhi::destroy_fence(rhi_fence* fence)
	{

	}

	void vulkan_rhi::destroy_device()
	{

	}

	void vulkan_rhi::destroy_command_pool(rhi_command_pool* command_pool)
	{

	}

	void vulkan_rhi::destroy_buffer(rhi_buffer*& buffer)
	{

	}

	void vulkan_rhi::free_command_buffer(rhi_command_pool* command_pool, uint32_t command_buffer_count, rhi_command_buffer* command_buffer)
	{

	}

	void vulkan_rhi::free_memory(rhi_device_memory*& memory)
	{

	}

	rhi_semaphore*& vulkan_rhi::get_texture_copy_semaphore(uint32_t index)
	{
		return m_image_available_for_texturescopy_semaphores_[index];
	}

	bool vulkan_rhi::is_point_light_shadow_enabled()
	{
		return false;
	}

	rhi_interface_type vulkan_rhi::get_interface_type() const
	{
		return rhi_interface_type::vulkan;
	}

	void vulkan_rhi::create_instance()
	{
		if (m_enable_validation_layers_ && !check_validation_layer_support())
		{
			SPDLOG_ERROR("validation layers requested, but not available!");
		}

		m_vulkan_api_version_ = VK_API_VERSION_1_3;

		// api info
		VkApplicationInfo app_info
		{
			.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
			.pApplicationName = "source_renderer",
			.applicationVersion = VK_MAKE_VERSION(1, 3, 0),
			.pEngineName = "source",
			.engineVersion = VK_MAKE_VERSION(1, 3, 0),
			.apiVersion = m_vulkan_api_version_
		};

		auto extensions = get_required_extensions();
		// create info
		VkInstanceCreateInfo instance_create_info
		{
			.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
			.pApplicationInfo = &app_info,
			.enabledExtensionCount = static_cast<uint32_t>(extensions.size()),
			.ppEnabledExtensionNames = extensions.data()
		};

		VkDebugUtilsMessengerCreateInfoEXT debug_create_info{};
		if (m_enable_validation_layers_)
		{
			instance_create_info.enabledLayerCount = static_cast<uint32_t>(m_validation_layers_.size());
			instance_create_info.ppEnabledLayerNames = m_validation_layers_.data();

			populate_debug_messenger_create_info(debug_create_info);
			instance_create_info.pNext = &debug_create_info;
		}
		else
		{
			instance_create_info.enabledLayerCount = 0;
			instance_create_info.pNext = nullptr;
		}

		if (vkCreateInstance(&instance_create_info, nullptr, &m_instance_))
		{
			SPDLOG_ERROR("vk create instance");
		}
	}

	void vulkan_rhi::initialize_debug_messenger()
	{
		if (m_enable_validation_layers_)
		{
			VkDebugUtilsMessengerCreateInfoEXT create_info;
			populate_debug_messenger_create_info(create_info);
			if (VK_SUCCESS != create_debug_utils_messenger_ext(m_instance_, &create_info, nullptr, &m_debug_messenger_))
			{
				SPDLOG_ERROR("failed to set up debug messenger!");
			}
		}

		if (m_enable_debug_utils_label_)
		{
			m_vk_cmd_begin_debug_utils_label_ext_ = reinterpret_cast<PFN_vkCmdBeginDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_instance_, "vkCmdBeginDebugUtilsLabelEXT"));
			m_vk_cmd_end_debug_utils_label_ext_ = reinterpret_cast<PFN_vkCmdEndDebugUtilsLabelEXT>(vkGetInstanceProcAddr(m_instance_, "vkCmdEndDebugUtilsLabelEXT"));
		}
	}

	void vulkan_rhi::create_window_surface()
	{
		if (glfwCreateWindowSurface(m_instance_, m_window, nullptr, &m_surface) != VK_SUCCESS)
		{
			SPDLOG_ERROR("glfwCreateWindowSurface failed!");
		}
	}

	void vulkan_rhi::initialize_physical_device()
	{
		uint32_t physical_device_count;
		vkEnumeratePhysicalDevices(m_instance_, &physical_device_count, nullptr);
		if (physical_device_count == 0)
		{
			SPDLOG_ERROR("enumerate physical devices failed!");
		}
		else
		{
			// find one device that matches our requirement
			// or find which is the best
			std::vector<VkPhysicalDevice> physical_devices(physical_device_count);
			vkEnumeratePhysicalDevices(m_instance_, &physical_device_count, physical_devices.data());

			std::vector<std::pair<int, VkPhysicalDevice>> ranked_physical_devices;
			for (const auto& device : physical_devices)
			{
				VkPhysicalDeviceProperties physical_device_properties;
				vkGetPhysicalDeviceProperties(device, &physical_device_properties);
				int score = 0;

				if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
				{
					score += 1000;
				}
				else if (physical_device_properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
				{
					score += 100;
				}

				auto& [f, s] = ranked_physical_devices.emplace_back();
				f = score;
				s = device;
			}

			std::ranges::sort(ranked_physical_devices.begin(),
				ranked_physical_devices.end(),
				[](const std::pair<int, VkPhysicalDevice>& p1, const std::pair<int, VkPhysicalDevice>& p2)
				{
					return p1 > p2;
				});

			for (const auto& [s, d] : ranked_physical_devices)
			{
				if (is_device_suitable(d))
				{
					m_physical_device = d;
					break;
				}
			}

			if (m_physical_device == VK_NULL_HANDLE)
			{
				SPDLOG_ERROR("failed to find suitable physical device");
			}
		}
	}

	void vulkan_rhi::create_logical_device()
	{
		m_queue_family_indices = find_queue_families(m_physical_device);

		std::vector<VkDeviceQueueCreateInfo> queue_create_infos;
		std::set<uint32_t> queue_families =
		{
			m_queue_family_indices.graphics_family.value(),
			m_queue_family_indices.present_family.value(),
			m_queue_family_indices.compute_family.value()
		};

		float queue_priority = 1.0f;
		for (uint32_t queue_family : queue_families)
		{
			VkDeviceQueueCreateInfo queue_create_info
			{
				.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO,
				.queueFamilyIndex = queue_family,
				.queueCount = 1,
				.pQueuePriorities = &queue_priority
			};
			queue_create_infos.push_back(queue_create_info);
		}

		// physical device features
		VkPhysicalDeviceFeatures physical_device_features{};
		physical_device_features.samplerAnisotropy = VK_TRUE;
		physical_device_features.fragmentStoresAndAtomics = VK_TRUE;
		physical_device_features.independentBlend = VK_TRUE;
		if (m_enable_point_light_shadow_)
		{
			physical_device_features.geometryShader = VK_TRUE;
		}

		// device create info
		VkDeviceCreateInfo device_create_info{};
		device_create_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
		device_create_info.pQueueCreateInfos = queue_create_infos.data();
		device_create_info.queueCreateInfoCount = static_cast<uint32_t>(queue_create_infos.size());
		device_create_info.pEnabledFeatures = &physical_device_features;
		device_create_info.enabledExtensionCount = static_cast<uint32_t>(m_device_extensions_.size());
		device_create_info.ppEnabledExtensionNames = m_device_extensions_.data();
		device_create_info.enabledLayerCount = 0;
		if (vkCreateDevice(m_physical_device, &device_create_info, nullptr, &m_device) != VK_SUCCESS)
		{
			SPDLOG_ERROR("vk create device");
		}

		// 从逻辑设备中获取一个可用的队列，以便后续的命令提交
		VkQueue vk_graphics_queue;
		vkGetDeviceQueue(m_device, m_queue_family_indices.graphics_family.value(), 0, &vk_graphics_queue);
		m_graphics_queue = new vulkan_rhi_queue();
		static_cast<vulkan_rhi_queue*>(m_graphics_queue)->set_resource(vk_graphics_queue);
	}

	void vulkan_rhi::destroy_image_view(rhi_image_view* image_view)
	{
		vkDestroyImageView(m_device, static_cast<vulkan_rhi_image_view*>(image_view)->get_resource(), nullptr);
	}

	bool vulkan_rhi::check_validation_layer_support() const
	{
		// 查询可用的实例层数量
		uint32_t layer_count;
		vkEnumerateInstanceLayerProperties(&layer_count, nullptr);

		// 创建一个数组来存储属性
		std::vector<VkLayerProperties> available_layers(layer_count);
		vkEnumerateInstanceLayerProperties(&layer_count, available_layers.data());

		for (const char* layer_name : m_validation_layers_)
		{
			bool layer_found = false;

			for (const auto& layer_properties : available_layers)
			{
				if (strcmp(layer_name, layer_properties.layerName) == 0)
				{
					layer_found = true;
					break;
				}
			}

			if (!layer_found)
			{
				return false;
			}
		}
		return true;
	}

	std::vector<char const*> vulkan_rhi::get_required_extensions()
	{
		uint32_t glfw_extension_count = 0;
		const char** glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);

		std::vector<const char*> extensions(glfw_extensions, glfw_extensions + glfw_extension_count);

		if (m_enable_validation_layers_ || m_enable_debug_utils_label_)
		{
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debug_callback(VkDebugUtilsMessageSeverityFlagBitsEXT,
		VkDebugUtilsMessageTypeFlagsEXT,
		const VkDebugUtilsMessengerCallbackDataEXT* callback_data,
		void*)
	{
		std::cerr << "validation layer: " << callback_data->pMessage << std::endl;
		return VK_FALSE;
	}

	void vulkan_rhi::populate_debug_messenger_create_info(VkDebugUtilsMessengerCreateInfoEXT& create_info)
	{
		create_info =
		{
			.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT,
			.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT,
			.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT,
			.pfnUserCallback = debug_callback
		};
	}

	VkResult vulkan_rhi::create_debug_utils_messenger_ext(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* create_info, const VkAllocationCallbacks* allocator, VkDebugUtilsMessengerEXT* debug_utils_messenger)
	{
		if (const auto func = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT")); func != nullptr)
		{
			return func(instance, create_info, allocator, debug_utils_messenger);
		}
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}

	bool vulkan_rhi::is_device_suitable(const VkPhysicalDevice physical_device)
	{
		auto queue_indices = find_queue_families(physical_device);
		bool is_swap_chain_adequate = false;
		if (const bool is_extensions_supported = check_device_extension_support(physical_device); is_extensions_supported)
		{
			swap_chain_support_details swap_chain_support_details = query_swap_chain_support(physical_device);
			is_swap_chain_adequate = !swap_chain_support_details.format.empty() && !swap_chain_support_details.present_mode.empty();
		}

		VkPhysicalDeviceFeatures physical_device_features;
		vkGetPhysicalDeviceFeatures(physical_device, &physical_device_features);

		if (!queue_indices.is_complete() || !is_swap_chain_adequate || !physical_device_features.samplerAnisotropy)
		{
			return false;
		}
		return true;
	}

	swap_chain_support_details vulkan_rhi::query_swap_chain_support(VkPhysicalDevice physical_device) const
	{
		swap_chain_support_details details;

		// 获取物理设备表面支持信息
		vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physical_device, m_surface, &details.capabilities);

		uint32_t format_count;
		vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &format_count, nullptr);
		if (format_count != 0)
		{
			details.format.resize(format_count);
			vkGetPhysicalDeviceSurfaceFormatsKHR(physical_device, m_surface, &format_count, details.format.data());
		}

		uint32_t present_mode_count;
		vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, nullptr);
		if (present_mode_count != 0)
		{
			details.present_mode.resize(present_mode_count);
			vkGetPhysicalDeviceSurfacePresentModesKHR(physical_device, m_surface, &present_mode_count, details.present_mode.data());
		}
		return details;
	}

	queue_family_indices vulkan_rhi::find_queue_families(const VkPhysicalDevice physical_device) const
	{
		queue_family_indices indices;
		uint32_t queue_family_count = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, nullptr);
		std::vector<VkQueueFamilyProperties> queue_family_properties(queue_family_count);
		vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_family_count, queue_family_properties.data());

		int i = 0;
		for (const auto& queue_family : queue_family_properties)
		{
			// 图形渲染和相关操作
			if (queue_family.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
			}
			// 计算任务和计算着色器
			if (queue_family.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				indices.compute_family = i;
			}

			VkBool32 is_present_support = false;
			vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, m_surface, &is_present_support);

			if (is_present_support)
			{
				indices.present_family = i;
			}

			if (indices.is_complete())
			{
				break;
			}
			i++;
		}
		return indices;
	}

	bool vulkan_rhi::check_device_extension_support(const VkPhysicalDevice physical_device)
	{
		uint32_t extension_count;
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, nullptr);

		std::vector<VkExtensionProperties> available_extensions(extension_count);
		vkEnumerateDeviceExtensionProperties(physical_device, nullptr, &extension_count, available_extensions.data());

		std::set<std::string> required_extensions(m_device_extensions_.begin(), m_device_extensions_.end());
		for (const auto& [extension_name, spec_version] : available_extensions)
		{
			required_extensions.erase(extension_name);
		}

		return required_extensions.empty();
	}

	VkSurfaceFormatKHR vulkan_rhi::choose_swap_chain_surface_format_form_details(const std::vector<VkSurfaceFormatKHR>& available_surface_formats)
	{
		for (const auto surface_format : available_surface_formats)
		{
			if (surface_format.format == VK_FORMAT_B8G8R8A8_UNORM && surface_format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
			{
				return surface_format;
			}
		}
		return available_surface_formats[0];
	}

	VkPresentModeKHR vulkan_rhi::choose_swap_chain_present_mode_from_details(const std::vector<VkPresentModeKHR>& available_present_modes)
	{
		for (const VkPresentModeKHR available_present_mode : available_present_modes)
		{
			if (available_present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
			{
				// 用于低延迟和高帧率的应用，可能会导致撕裂
				return VK_PRESENT_MODE_MAILBOX_KHR;
			}
		}
		// 垂直同步模式，图像的城乡会被限制在显示器的刷新率内
		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D vulkan_rhi::choose_swap_chain_extent_from_details(const VkSurfaceCapabilitiesKHR& capabilities) const
	{
		if (capabilities.currentExtent.width != UINT32_MAX)
		{
			return capabilities.currentExtent;
		}
		int width, height;
		glfwGetFramebufferSize(m_window, &width, &height);

		VkExtent2D actual_extent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

		actual_extent.width = std::clamp(actual_extent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		actual_extent.height = std::clamp(actual_extent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
		return actual_extent;
	}

	rhi_compute_context* vulkan_rhi::rhi_get_command_context(rhi_pipeline_type pipeline, rhi_gpu_mask gpu_mask)
	{
		return nullptr;
	}

}
