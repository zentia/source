#include "vulkan_rhi.h"

#include <iostream>

#include "Editor/Src/Application/Application.h"
#include "interface/vulkan/vulkan_rhi_resource.h"

namespace source_runtime
{
	void vulkan_rhi::initialize(const rhi_init_info init_info)
	{
		m_window = init_info.window_system->GetWindow();

		const std::array<int, 2> window_size = init_info.window_system->GetWindowSize();
		const float width = static_cast<float>(window_size[0]);
		const float height = static_cast<float>(window_size[1]);
		m_viewport = { 0.0f,0.0f, width, height, 0.0f, 1.0f };
		m_scissor = { {0,0},	{(uint32_t)(window_size[0]), (uint32_t)(window_size[1])} };
#ifdef DEBUG
		m_enable_validation_layers_ = true;
		m_enable_debug_utils_label_ = true;
#else
		m_enable_debug_utils_label_ = false;
		m_enable_debug_utils_label_ = false;
#endif
#ifdef _MSC_VER
		char buffer[4096];
		if (const DWORD result = GetEnvironmentVariable("VK_SDK_PATH", buffer, sizeof(buffer)); result > 0 && result < sizeof(buffer))
		{
			const std::filesystem::path path = buffer;
			SetEnvironmentVariable("VK_LAYER_PATH", (path / "Bin").generic_string().c_str());
		}
		SetEnvironmentVariable("DISABLE_LAYER_AMD_SWITCHABLE_GRAPHICS_1", "1");
#endif

		create_instance();
		initialize_debug_messenger();
		create_window_surface();
		initialize_physical_device();
		create_logical_device();
	}

	void vulkan_rhi::create_instance()
	{
		if (m_enable_validation_layers_ && !check_validation_layer_support())
		{
			LOG_ERROR("validation layers requested, but not available!");
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
			LOG_ERROR("vk create instance");
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
				LOG_ERROR("failed to set up debug messenger!");
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
			LOG_ERROR("glfwCreateWindowSurface failed!");
		}
	}

	void vulkan_rhi::initialize_physical_device()
	{
		uint32_t physical_device_count;
		vkEnumeratePhysicalDevices(m_instance_, &physical_device_count, nullptr);
		if (physical_device_count == 0)
		{
			LOG_ERROR("enumerate physical devices failed!");
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
				LOG_ERROR("failed to find suitable physical device");
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
			LOG_ERROR("vk create device");
		}

		// 从逻辑设备中获取一个可用的队列，以便后续的命令提交
		VkQueue vk_graphics_queue;
		vkGetDeviceQueue(m_device, m_queue_family_indices.graphics_family.value(), 0, &vk_graphics_queue);
		m_graphics_queue = new vulkan_rhi_queue();
		static_cast<vulkan_rhi_queue*>(m_graphics_queue)->set_resource(vk_graphics_queue);
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

}
