#include "vulkan_rhi.h"

#include <stb_image.h>
#include "function/global/global_context.h"
#include <editor/include/editor_global_context.h>
#include <editor/include/editor_input_manager.h>
#include <editor/include/editor_scene_manager.h>
#include <function/render/window_system.h>
#include "function/render/render_camera.h"
#include <filesystem>
#include <fstream>
#include <ostream>
#include "function/render/rhi/graphics.h"
#include <algorithm>
#ifdef USE_VOLK
#define VOLK_IMPLEMENTATION
#include "vulkan/volk.h"

#define VMA_IMPLEMENTATION
#define VMA_STATIC_VULKAN_FUNCTIONS 0
#define VMA_DYNAMIC_VULKAN_FUNCTIONS 1
#include <vulkan/vk_mem_alloc.h>

#endif // USE_VOLK

#include <tiny_obj_loader.h>
#include <core/base/macro.h>

#include "core/helper.h"

#include "core/utils/spirv_reflect.h"

using namespace qyhs::graphics;

namespace qyhs
{
	enum
	{
		VULKAN_BINDING_SHIFT_B = 0,
		VULKAN_BINDING_SHIFT_T = 1000,
		VULKAN_BINDING_SHIFT_U = 2000,
		VULKAN_BINDING_SHIFT_S = 3000,
	};

	VkBorderColor convertSamplerBorderColor(const SamplerBorderColor& border_color);
	VkCompareOp convertCompareFunction(ComparisonFunc comparion_func);

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	//void Texture2D::fromBuffer(unsigned char* buffer, unsigned int buffer_size, VkFormat format, unsigned int width, unsigned int height, VkDevice device)
	//{
	//	assert(buffer);
	//	uint32_t mip_levels = std::floor(std::log2(width > height ? width : height));
	//
	//	VkBufferCreateInfo buffer_create_info{};
	//	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	//	buffer_create_info.size = static_cast<VkDeviceSize>(buffer_size);
	//	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	//	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	//
	//	VkBuffer stage_buffer;
	//
	//}

	struct Buffer_Vulkan
	{
		VkBuffer resource = VK_NULL_HANDLE;
		VmaAllocation allocation = nullptr;
		std::shared_ptr<VulkanRHI::AllocationHandler> allocation_handler = nullptr;
		struct BufferSubresource
		{
			int index = -1; //-1 for bindless
			VkDescriptorBufferInfo buffer_info{};
			inline bool isValid() { return index >= 0; }
			bool is_typed = false; //false for storage buffer,true for texel buffer;
			VkBufferView buffer_view;
		};

		void destroy_subresources()
		{
			uint64_t frame_count = allocation_handler->frame_count;
			if (this->srv.isValid())
			{
				if (srv.is_typed)
				{
					allocation_handler->destroyer_bufferviews.push_back(std::make_pair(srv.buffer_view, frame_count));
					allocation_handler->destroyer_bindless_uniform_texel_buffers.push_back(std::make_pair(srv.index, frame_count));
				}
				else
				{
					allocation_handler->destroyer_bindless_storage_buffers.push_back(std::make_pair(srv.index, frame_count));
				}
				srv = {};
			}

			if (uav.isValid())
			{
				if (uav.is_typed)
				{
					allocation_handler->destroyer_bufferviews.push_back(std::make_pair(uav.buffer_view, frame_count));
					allocation_handler->destroyer_bindless_storage_texel_buffers.push_back(std::make_pair(uav.index, frame_count));
				}
				else
				{
					allocation_handler->destroyer_bindless_storage_buffers.push_back(std::make_pair(uav.index, frame_count));
				}
				uav = {};
			}
			for (auto& x : subresources_srv)
			{
				if (srv.is_typed)
				{
					allocation_handler->destroyer_bufferviews.push_back(std::make_pair(srv.buffer_view, frame_count));
					allocation_handler->destroyer_bindless_uniform_texel_buffers.push_back(std::make_pair(srv.index, frame_count));
				}
				else
				{
					allocation_handler->destroyer_bindless_storage_buffers.push_back(std::make_pair(srv.index, frame_count));
				}
			}
			subresources_srv.clear();
			for (auto& x : subresources_uav)
			{
				if (x.is_typed)
				{
					allocation_handler->destroyer_bufferviews.push_back(std::make_pair(x.buffer_view, frame_count));
					allocation_handler->destroyer_bindless_storage_texel_buffers.push_back(std::make_pair(x.index, frame_count));
				}
				else
				{
					allocation_handler->destroyer_bindless_storage_buffers.push_back(std::make_pair(x.index, frame_count));
				}
			}
			subresources_uav.clear();

		}
		~Buffer_Vulkan()
		{
			if (allocation_handler == nullptr)
			{
				return;
			}
			allocation_handler->destroy_locker.lock();
			uint64_t frame_count = allocation_handler->frame_count;
			if (resource)
			{
				allocation_handler->destroyer_buffers.push_back(std::make_pair(std::make_pair(resource, allocation), frame_count));
			}
			destroy_subresources();
			allocation_handler->destroy_locker.unlock();
		}

		BufferSubresource srv;
		std::vector<BufferSubresource> subresources_srv;

		BufferSubresource uav;
		std::vector<BufferSubresource> subresources_uav;
	};

	std::string getCachePath()
	{
		return helper::getCacheDirectoryPath() + "/qyhsPipelineCache_Vulkan";
	}

	VulkanRHI::VulkanRHI()
	{
		m_physical_device_properties_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2;
		m_physical_device_properties_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_PROPERTIES;
		m_physical_device_properties_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_PROPERTIES;
		m_physical_device_properties_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_PROPERTIES;
		m_physical_device_properties_2.pNext = &m_physical_device_properties_1_1;
		m_physical_device_properties_1_1.pNext = &m_physical_device_properties_1_2;
		m_physical_device_properties_1_2.pNext = &m_physical_device_properties_1_3;

		m_physical_device_features_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
		m_physical_device_features_1_1.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
		m_physical_device_features_1_2.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES;
		m_physical_device_features_1_3.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES;
		m_physical_device_features_2.pNext = &m_physical_device_features_1_1;
		m_physical_device_features_1_1.pNext = &m_physical_device_features_1_2;
		m_physical_device_features_1_2.pNext = &m_physical_device_features_1_3;

		if (m_physical_device_features_1_2.samplerFilterMinmax == VK_TRUE)
		{
			capabilities |= GraphicsDeviceCapability::SAMPLER_MINMAX;
		}

		initVolk();
		createInstance();
		setupDebugMessenger();
		initInstanceFunction();
		pickPhysicalDevice();
		createLogicalDevice();

		VkFenceCreateInfo fence_create_info = {};
		fence_create_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fence_create_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		for (int i = 0; i < BUFFER_COUNT; ++i)
		{
			for (int j = 0; j < QUEUE_COUNT; ++j)
			{
				assert(vkCreateFence(m_device, &fence_create_info, nullptr, &frame_fences[i][j]) == VK_SUCCESS);
				assert(vkResetFences(m_device, 1, &frame_fences[i][j]) == VK_SUCCESS);
			}
		}

		allocation_handler = std::make_shared<AllocationHandler>();
		allocation_handler->device = m_device;

		if (m_physical_device_features_1_2.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE)
		{
			allocation_handler->bindless_sampled_images.init(m_device, VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE, m_physical_device_properties_1_2.maxDescriptorSetUpdateAfterBindSampledImages / 4);
		}
		if (m_physical_device_features_1_2.descriptorBindingSampledImageUpdateAfterBind == VK_TRUE)
		{
			allocation_handler->bindless_samplers.init(m_device, VK_DESCRIPTOR_TYPE_SAMPLER, 256);
		}
		if (m_physical_device_features_1_2.descriptorBindingStorageImageUpdateAfterBind == VK_TRUE)
		{
			allocation_handler->bindless_storage_images.init(m_device, VK_DESCRIPTOR_TYPE_STORAGE_IMAGE, m_physical_device_properties_1_2.maxDescriptorSetUpdateAfterBindStorageImages / 4);
		}
		if (m_physical_device_features_1_2.descriptorBindingStorageBufferUpdateAfterBind == VK_TRUE)
		{
			allocation_handler->bindless_storage_buffers.init(m_device, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, m_physical_device_properties_1_2.maxDescriptorSetUpdateAfterBindStorageBuffers / 4);
		}
		if (m_physical_device_features_1_2.descriptorBindingUniformTexelBufferUpdateAfterBind == VK_TRUE)
		{
			allocation_handler->bindless_uniform_texel_buffers.init(m_device, VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER, m_physical_device_properties_1_2.maxDescriptorSetUpdateAfterBindStorageBuffers / 4);
		}

		//create allocator
		VkResult res;
		VmaAllocatorCreateInfo allocate_info{};
		allocate_info.physicalDevice = physical_device;
		allocate_info.device = m_device;
		allocate_info.instance = instance;

		allocate_info.flags =
			VMA_ALLOCATOR_CREATE_KHR_DEDICATED_ALLOCATION_BIT |
			VMA_ALLOCATOR_CREATE_KHR_BIND_MEMORY2_BIT;

		if (m_physical_device_features_1_2.bufferDeviceAddress)
		{
			allocate_info.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;
		}

#if VMA_DYNAMIC_VULKAN_FUNCTIONS
		static VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
		allocate_info.pVulkanFunctions = &vulkanFunctions;
#endif
		res = vmaCreateAllocator(&allocate_info, &allocation_handler->allocator);
		assert(res == VK_SUCCESS);

		// Dynamic PSO states:
		pso_dynamic_states.push_back(VK_DYNAMIC_STATE_VIEWPORT_WITH_COUNT);
		pso_dynamic_states.push_back(VK_DYNAMIC_STATE_SCISSOR_WITH_COUNT);
		pso_dynamic_states.push_back(VK_DYNAMIC_STATE_STENCIL_REFERENCE);
		pso_dynamic_states.push_back(VK_DYNAMIC_STATE_BLEND_CONSTANTS);
		if (checkCapability(GraphicsDeviceCapability::DEPTH_BOUNDS_TEST))
		{
			pso_dynamic_states.push_back(VK_DYNAMIC_STATE_DEPTH_BOUNDS);
		}
		if (checkCapability(GraphicsDeviceCapability::VARIABLE_RATE_SHADING))
		{
			pso_dynamic_states.push_back(VK_DYNAMIC_STATE_FRAGMENT_SHADING_RATE_KHR);
		}
		//pso_dynamic_states.push_back(VK_DYNAMIC_STATE_VERTEX_INPUT_BINDING_STRIDE);
		dynamic_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamic_state_info.pDynamicStates = pso_dynamic_states.data();
		dynamic_state_info.dynamicStateCount = pso_dynamic_states.size();

		//Pipeline Cache
		{
			std::vector<uint8_t> pipeline_data;
			if (!helper::readFile(getCachePath(), pipeline_data))
			{
				pipeline_data.clear();
			}
			// Verify cache validation.
			if (!pipeline_data.empty())
			{
				uint32_t headerLength = 0;
				uint32_t cacheHeaderVersion = 0;
				uint32_t vendorID = 0;
				uint32_t deviceID = 0;
				uint8_t pipelineCacheUUID[VK_UUID_SIZE] = {};

				std::memcpy(&headerLength, (uint8_t*)pipeline_data.data() + 0, 4);
				std::memcpy(&cacheHeaderVersion, (uint8_t*)pipeline_data.data() + 4, 4);
				std::memcpy(&vendorID, (uint8_t*)pipeline_data.data() + 8, 4);
				std::memcpy(&deviceID, (uint8_t*)pipeline_data.data() + 12, 4);
				std::memcpy(pipelineCacheUUID, (uint8_t*)pipeline_data.data() + 16, VK_UUID_SIZE);

				bool badCache = false;

				if (headerLength <= 0)
				{
					badCache = true;
				}

				if (cacheHeaderVersion != VK_PIPELINE_CACHE_HEADER_VERSION_ONE)
				{
					badCache = true;
				}

				if (vendorID != m_physical_device_properties_2.properties.vendorID)
				{
					badCache = true;
				}

				if (deviceID != m_physical_device_properties_2.properties.deviceID)
				{
					badCache = true;
				}

				if (memcmp(pipelineCacheUUID, m_physical_device_properties_2.properties.pipelineCacheUUID, sizeof(pipelineCacheUUID)) != 0)
				{
					badCache = true;
				}

				if (badCache)
				{
					// Don't submit initial cache data if any version info is incorrect
					pipeline_data.clear();
				}
			}

			VkPipelineCacheCreateInfo cache_info = {};
			cache_info.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
			cache_info.initialDataSize = pipeline_data.size();
			cache_info.pInitialData = pipeline_data.data();
			assert(vkCreatePipelineCache(m_device, &cache_info, nullptr, &pipeline_cache) == VK_SUCCESS);

			copy_allocator.init(this);
		}

		//create static sampler
		{
			VkSamplerCreateInfo createInfo = {};
			createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			createInfo.pNext = nullptr;
			createInfo.flags = 0;
			createInfo.compareEnable = false;
			createInfo.compareOp = VK_COMPARE_OP_NEVER;
			createInfo.minLod = 0;
			createInfo.maxLod = FLT_MAX;
			createInfo.mipLodBias = 0;
			createInfo.anisotropyEnable = false;
			createInfo.maxAnisotropy = 0;

			// sampler_linear_clamp:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_linear_wrap:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			//sampler_linear_mirror:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_point_clamp:
			createInfo.minFilter = VK_FILTER_NEAREST;
			createInfo.magFilter = VK_FILTER_NEAREST;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_point_wrap:
			createInfo.minFilter = VK_FILTER_NEAREST;
			createInfo.magFilter = VK_FILTER_NEAREST;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_point_mirror:
			createInfo.minFilter = VK_FILTER_NEAREST;
			createInfo.magFilter = VK_FILTER_NEAREST;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_aniso_clamp:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.anisotropyEnable = true;
			createInfo.maxAnisotropy = 16;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_aniso_wrap:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			createInfo.anisotropyEnable = true;
			createInfo.maxAnisotropy = 16;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_aniso_mirror:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			createInfo.anisotropyEnable = true;
			createInfo.maxAnisotropy = 16;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);

			// sampler_cmp_depth:
			createInfo.minFilter = VK_FILTER_LINEAR;
			createInfo.magFilter = VK_FILTER_LINEAR;
			createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			createInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			createInfo.anisotropyEnable = false;
			createInfo.maxAnisotropy = 0;
			createInfo.compareEnable = true;
			createInfo.compareOp = VK_COMPARE_OP_GREATER_OR_EQUAL;
			createInfo.minLod = 0;
			createInfo.maxLod = 0;
			res = vkCreateSampler(m_device, &createInfo, nullptr, &immutable_samplers.emplace_back());
			assert(res == VK_SUCCESS);
		}
	}

	VulkanRHI::~VulkanRHI()
	{
		//cleanup();
	}
	void VulkanRHI::initialize()
	{
		m_window = g_runtime_global_context.m_window_system->getWindow();
		initVulkan();
		current_time = glfwGetTime();
	}

	void VulkanRHI::initialize2()
	{
		initVulkan2();
	}

	void VulkanRHI::cleanup() {
		cleanupSwapChain();

		vkDestroyDescriptorPool(m_device, m_descriptor_pool, nullptr);

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vkDestroySemaphore(m_device, renderFinishedSemaphores[i], nullptr);
			vkDestroySemaphore(m_device, imageAvailableSemaphores[i], nullptr);
			vkDestroyFence(m_device, m_is_frame_in_flight_fences[i], nullptr);
		}

		for (size_t i = 0; i < m_command_buffers.size(); ++i)
		{
			vkFreeCommandBuffers(m_device, m_command_pools[i], 1U, &m_command_buffers[i]);
			vkDestroyCommandPool(m_device, m_command_pools[i], nullptr);
		}

		vkDestroyDevice(m_device, nullptr);

		if (enableValidationLayers) {
			DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		vkDestroySurfaceKHR(instance, surface, nullptr);
		vkDestroyInstance(instance, nullptr);

		glfwDestroyWindow(m_window);

		glfwTerminate();
	}

	void VulkanRHI::initVulkan2()
	{
		initVolk();
		createInstance();
		setupDebugMessenger();
		initInstanceFunction();
		//createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createViewport();
		createCommandPool();
		createDepthResources();
		createDescriptorPool();
		createCommandBuffers();
		createSyncObjects();
		createAllocator();

	}

	void VulkanRHI::initVulkan()
	{
		initVolk();
		createInstance();
		setupDebugMessenger();
		initInstanceFunction();
		createSurface();
		pickPhysicalDevice();
		createLogicalDevice();
		createSwapChain();
		createImageViews();
		createViewport();
		createCommandPool();
		createDepthResources();
		createDescriptorPool();
		createCommandBuffers();
		createSyncObjects();
		createAllocator();

	}

	void VulkanRHI::initVolk()
	{
#ifndef USE_VOLK
		return;
#else
		VkResult result = volkInitialize();
		assert(result == VK_SUCCESS);
#endif // !USE_VOLK

	}

	bool VulkanRHI::checkValidationLayerSupport() {
		uint32_t layerCount;
		VkResult result;
		result = vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
		assert(result == VK_SUCCESS);
		std::vector<VkLayerProperties> availableLayers(layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());
		assert(result == VK_SUCCESS);

		for (const char* layerName : validationLayers) {
			bool layerFound = false;

			for (const auto& layerProperties : availableLayers) {
				if (strcmp(layerName, layerProperties.layerName) == 0) {
					layerFound = true;
					break;
				}
			}

			if (!layerFound) {
				return false;
			}
		}

		return true;
	}

	VkSampler VulkanRHI::m_nearest_sampler = VK_NULL_HANDLE;


	VkShaderModule VulkanRHI::createShaderModule(VkDevice device, const std::vector<char>& code)
	{
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}

	void VulkanRHI::createBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
		VkBufferCreateInfo bufferInfo{};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = size;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physical_device, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);

	}

	CommandList VulkanRHI::beginCommandList(QueueType queue)
	{
		VkResult result;
		cmd_locker.lock();
		uint32_t current_cmd = cmd_count++;
		if (current_cmd >= commandlists.size())
		{
			commandlists.push_back(std::make_unique<CommandList_Vulkan>());
		}
		CommandList cmd;
		cmd.internal_state = commandlists[current_cmd].get();
		cmd_locker.unlock();

		CommandList_Vulkan& cmd_vulkan = getCommandList(cmd);
		cmd_vulkan.reset(getBufferIndex());
		cmd_vulkan.queue = queue;
		cmd_vulkan.index = current_cmd;
		if (cmd_vulkan.getCommandBuffer() == VK_NULL_HANDLE)
		{
			for (int i = 0; i < BUFFER_COUNT; ++i)
			{
				VkCommandPoolCreateInfo pool_info = {};
				pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				switch (queue)
				{
				case QUEUE_GRAPHICS:
					pool_info.queueFamilyIndex = queue_family_indices.graphics_family.value();
					break;
				case QUEUE_COMPUTE:
					pool_info.queueFamilyIndex = queue_family_indices.compute_family.value();
					break;
				case QUEUE_TRANSFER:
					pool_info.queueFamilyIndex = queue_family_indices.copy_family.value();
				default:
					assert(0); // queue type not handled
					break;
				}
				pool_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

				result = vkCreateCommandPool(m_device, &pool_info, nullptr, &cmd_vulkan.command_pools[i][queue]);
				assert(result == VK_SUCCESS);

				VkCommandBufferAllocateInfo allocate_info = {};
				allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocate_info.commandBufferCount = 1;
				allocate_info.commandPool = cmd_vulkan.command_pools[i][queue];
				allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				assert(vkAllocateCommandBuffers(m_device, &allocate_info, &cmd_vulkan.command_buffers[i][queue]) == VK_SUCCESS);

				cmd_vulkan.binder_pools[i].init(this);
			}
			cmd_vulkan.binder.init(this);
		}
		result = vkResetCommandPool(m_device, cmd_vulkan.getCommandPool(), 0);
		assert(result == VK_SUCCESS);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		begin_info.pInheritanceInfo = nullptr; // Optional

		auto cb = cmd_vulkan.getCommandBuffer();
		assert(cb != VK_NULL_HANDLE); 
		result = vkBeginCommandBuffer(cb, &begin_info);
		assert(result == VK_SUCCESS);
		return cmd;
	}

	Shader_Vulkan* to_internal(const Shader* shader)
	{
		return static_cast<Shader_Vulkan*>(shader->internal_state.get());
	}

	Sampler_Vulkan* to_internal(const Sampler* sampler)
	{
		return static_cast<Sampler_Vulkan*>(sampler->internal_state.get());
	}

	PipelineState_Vulkan* to_internal(const PipelineState* pipeline_state_object)
	{
		return static_cast<PipelineState_Vulkan*>(pipeline_state_object->internal_state.get());
	}

	Texture_Vulkan* to_internal(const Texture* texture)
	{
		return static_cast<Texture_Vulkan*>(texture->internal_state.get());
	}

	Buffer_Vulkan* to_internal(const GPUBuffer* buffer)
	{
		return static_cast<Buffer_Vulkan*>(buffer->internal_state.get());
	}

	constexpr VkComponentSwizzle _convertComponentSwizzle(ComponentSwizzle value)
	{
		switch (value)
		{
		default:
			return VK_COMPONENT_SWIZZLE_IDENTITY;
		case graphics::ComponentSwizzle::R:
			return VK_COMPONENT_SWIZZLE_R;
		case graphics::ComponentSwizzle::G:
			return VK_COMPONENT_SWIZZLE_G;
		case graphics::ComponentSwizzle::B:
			return VK_COMPONENT_SWIZZLE_B;
		case graphics::ComponentSwizzle::A:
			return VK_COMPONENT_SWIZZLE_A;
		case graphics::ComponentSwizzle::ZERO:
			return VK_COMPONENT_SWIZZLE_ZERO;
		case graphics::ComponentSwizzle::ONE:
			return VK_COMPONENT_SWIZZLE_ONE;
		}
	}

	constexpr VkComponentMapping _convertSwizzle(Swizzle value)
	{
		VkComponentMapping mapping = {};
		mapping.r = _convertComponentSwizzle(value.r);
		mapping.g = _convertComponentSwizzle(value.g);
		mapping.b = _convertComponentSwizzle(value.b);
		mapping.a = _convertComponentSwizzle(value.a);
		return mapping;
	}

	constexpr VkImageAspectFlags _convertImageAspectMask(ImageAspect value)
	{
		switch (value)
		{
		default:
		case graphics::ImageAspect::COLOR:
			return VK_IMAGE_ASPECT_COLOR_BIT;
		case graphics::ImageAspect::DEPTH:
			return VK_IMAGE_ASPECT_DEPTH_BIT;
		case graphics::ImageAspect::STENCIL:
			return VK_IMAGE_ASPECT_STENCIL_BIT;
		case graphics::ImageAspect::LUMINANCE:
			return VK_IMAGE_ASPECT_PLANE_0_BIT;
		case graphics::ImageAspect::CHROMINANCE:
			return VK_IMAGE_ASPECT_PLANE_1_BIT;
		}
	}

	VkImageView VulkanRHI::createImageView(VkDevice device, VkImage image, VkFormat format, VkImageViewType image_view_type, uint32_t layer_count, uint32_t mip_levels, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = image_view_type;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mip_levels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = layer_count;

		VkImageView imageView;
		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	void VulkanRHI::transitionImageLayout(RHI* rhi, VkImage image, VkFormat format, uint32_t layer_count, uint32_t mip_levels, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		VkCommandBuffer commandBuffer = vulkan_rhi->beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = layer_count;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		vulkan_rhi->endSingleTimeCommands(commandBuffer);
	}

	void VulkanRHI::copyBufferToImage(RHI* rhi, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t layer_count) {
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		VkCommandBuffer commandBuffer = vulkan_rhi->beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = layer_count;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		vulkan_rhi->endSingleTimeCommands(commandBuffer);
	}

	void VulkanRHI::createTextureImageView(VkDevice device, VkImage image, VkImageView& image_view, uint32_t mip_levels) {
		image_view = createImageView(device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D, 1, mip_levels, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanRHI::copyBuffer(RHI* rhi, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize src_offset, VkDeviceSize dst_offset, VkDeviceSize size)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		VkCommandBuffer commandBuffer = vulkan_rhi->beginSingleTimeCommands();

		VkBufferCopy copyRegion{ src_offset,dst_offset,size };
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vulkan_rhi->endSingleTimeCommands(commandBuffer);

	}

	void VulkanRHI::generateMipmaps(RHI* rhi, VkImage image, VkFormat format, uint32_t tex_width, uint32_t tex_height, uint32_t mip_levels)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		VkPhysicalDevice physical_device = vulkan_rhi->getPhysicalDevice();
		VkDevice		 device = vulkan_rhi->getDevice();
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);
		if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			throw std::runtime_error("texture image doesn't support linear blitting!");
		}
		VkCommandBuffer command_buffer = vulkan_rhi->beginSingleTimeCommands();
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int mip_width = tex_width;
		int mip_height = tex_height;
		for (uint32_t i = 1; i < mip_levels; ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0,0,0 };
			blit.srcOffsets[1] = { mip_width,mip_height,1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.srcSubresource.mipLevel = i - 1;

			blit.dstOffsets[0] = { 0,0,0 };
			blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1,mip_height > 1 ? mip_height / 2 : 1,1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.layerCount = 1;


			vkCmdBlitImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
			if (mip_width > 1) mip_width /= 2;
			if (mip_height > 1)mip_height /= 2;
		}
		barrier.subresourceRange.baseMipLevel = mip_levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		vulkan_rhi->endSingleTimeCommands(command_buffer);
	}

	VkSampler& VulkanRHI::getOrCreateMipMapSampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t mip_levels) {
		auto iter = m_mipmap_sampler_map.find(mip_levels);
		if (iter != m_mipmap_sampler_map.end())
		{
			return iter->second;
		}
		else
		{
			VkSampler sampler;
			VkPhysicalDeviceProperties properties{};
			vkGetPhysicalDeviceProperties(physical_device, &properties);

			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = VK_FILTER_LINEAR;
			samplerInfo.minFilter = VK_FILTER_LINEAR;
			samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.anisotropyEnable = VK_TRUE;
			samplerInfo.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
			samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			samplerInfo.unnormalizedCoordinates = VK_FALSE;
			samplerInfo.compareEnable = VK_FALSE;
			samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.minLod = 0; // Optional
			samplerInfo.maxLod = static_cast<float>(mip_levels);
			samplerInfo.mipLodBias = 0.0f; // Optional

			if (vkCreateSampler(device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
				throw std::runtime_error("failed to create texture sampler!");
			}
			m_mipmap_sampler_map.insert(std::make_pair(mip_levels, std::move(sampler)));
			return m_mipmap_sampler_map[mip_levels];
		}
	}

	void VulkanRHI::createCubeMap(RHI* rhi, VkImage& image, VkImageView& image_view, VmaAllocation& allocation, uint32_t width, uint32_t height, std::array<void*, 6> pixels, PIXEL_FORMAT texture_image_format, uint32_t miplevels)
	{
		VkDeviceSize cube_byte_size;
		VkDeviceSize texture_byte_size;
		VkFormat vulkan_image_format;
		switch (texture_image_format)
		{
		case qyhs::FORMAT_R8G8B8_UNORM:
			texture_byte_size = width * height * 3;
			vulkan_image_format = VK_FORMAT_R8G8_UNORM;
			break;
		case qyhs::FORMAT_R8G8B8_SRGB:
			texture_byte_size = width * height * 3;
			vulkan_image_format = VK_FORMAT_R8G8_SRGB;
			break;
		case qyhs::FORMAT_R8G8B8A8_UNORM:
			texture_byte_size = width * height * 4;
			vulkan_image_format = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case qyhs::FORMAT_R8G8B8A8_SRGB:
			texture_byte_size = width * height * 4;
			vulkan_image_format = VK_FORMAT_R8G8B8A8_SRGB;
			break;
		case qyhs::FORMAT_R32G32_FLOAT:
			texture_byte_size = width * height * 4 * 2;
			vulkan_image_format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case qyhs::FORMAT_R32G32B32_FLOAT:
			texture_byte_size = width * height * 4 * 3;
			vulkan_image_format = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case qyhs::FORMAT_R32G32B32A32_FLOAT:
			texture_byte_size = width * height * 4 * 4;
			vulkan_image_format = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;
		default:
			texture_byte_size = VkDeviceSize(-1);
			vulkan_image_format = VK_FORMAT_UNDEFINED;
			throw std::runtime_error("invalid texture byte size");
			break;
		}
		cube_byte_size = texture_byte_size * 6;

		VkImageCreateInfo cube_map_image_create_info{};
		cube_map_image_create_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		cube_map_image_create_info.flags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		cube_map_image_create_info.format = vulkan_image_format;
		cube_map_image_create_info.extent.depth = 1;
		cube_map_image_create_info.arrayLayers = 6;
		cube_map_image_create_info.imageType = VK_IMAGE_TYPE_2D;
		cube_map_image_create_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		cube_map_image_create_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		cube_map_image_create_info.extent.width = static_cast<uint32_t>(width);
		cube_map_image_create_info.extent.height = static_cast<uint32_t>(height);
		cube_map_image_create_info.mipLevels = miplevels;
		cube_map_image_create_info.usage =
			VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		cube_map_image_create_info.samples = VK_SAMPLE_COUNT_1_BIT;
		cube_map_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocate_info = {};
		allocate_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		vmaCreateImage(static_cast<VulkanRHI*>(rhi)->m_assets_allocator,
			&cube_map_image_create_info, &allocate_info, &image, &allocation, nullptr);

		VkBuffer storage_buffer;
		VkDeviceMemory storage_buffer_memory;
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), cube_byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storage_buffer, storage_buffer_memory);

		void* data;
		vkMapMemory(vulkan_rhi->getDevice(), storage_buffer_memory, 0, cube_byte_size, 0, &data);
		for (int i = 0; i < 6; ++i)
		{
			memcpy((void*)(static_cast<char*>(data) + texture_byte_size * i), pixels[i], static_cast<size_t>(texture_byte_size));

		}
		vkUnmapMemory(vulkan_rhi->getDevice(), storage_buffer_memory);
		transitionImageLayout(rhi, image, vulkan_image_format, 6, miplevels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(rhi, storage_buffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), 6);
		transitionImageLayout(rhi, image, vulkan_image_format, 6, miplevels, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		vkDestroyBuffer(vulkan_rhi->getDevice(), storage_buffer, nullptr);
		vkFreeMemory(vulkan_rhi->getDevice(), storage_buffer_memory, nullptr);

		image_view = createImageView(vulkan_rhi->getDevice(), image, vulkan_image_format, VK_IMAGE_VIEW_TYPE_CUBE, 6, miplevels, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	VkSampler VulkanRHI::getOrCreateNearestSampler(VkPhysicalDevice physical_device, VkDevice device)
	{
		if (m_nearest_sampler == VK_NULL_HANDLE)
		{
			VkPhysicalDeviceProperties physical_property;
			vkGetPhysicalDeviceProperties(physical_device, &physical_property);

			VkSamplerCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			create_info.mipLodBias = 0.0f;
			create_info.anisotropyEnable = VK_FALSE;
			create_info.maxAnisotropy = physical_property.limits.maxSamplerAnisotropy; // close :1.0f
			create_info.compareEnable = VK_FALSE;
			create_info.compareOp = VK_COMPARE_OP_ALWAYS;
			create_info.minLod = 0.0f;
			create_info.maxLod = 8.0f; // todo: m_irradiance_texture_miplevels
			create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
			create_info.unnormalizedCoordinates = VK_FALSE;

			vkCreateSampler(device, &create_info, nullptr, &m_nearest_sampler);
		}
		return m_nearest_sampler;
	}

	bool createInternalSwapChain(VulkanRHI* rhi, VulkanRHI::SwapChain_Vulkan* internal_state)
	{
		VkResult result;
		SwapChainSupportDetails details = querySwapChainSupport(rhi->physical_device, internal_state->surface);

		VkSurfaceCapabilitiesKHR swapchain_capabilities;
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(rhi->physical_device, internal_state->surface, &swapchain_capabilities);
		assert(result == VK_SUCCESS);

		uint32_t surface_format_count;
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(rhi->physical_device, internal_state->surface, &surface_format_count, nullptr);
		assert(result == VK_SUCCESS);
		std::vector<VkSurfaceFormatKHR> available_surface_formats(surface_format_count);
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(rhi->physical_device, internal_state->surface, &surface_format_count, available_surface_formats.data());
		assert(result == VK_SUCCESS);

		//default surface format
		VkSurfaceFormatKHR surface_format = chooseSwapSurfaceFormat(available_surface_formats);
		internal_state->desc.format = Format::B8G8R8A8_UNORM;
		surface_format.format = VK_FORMAT_B8G8R8A8_UNORM;
		surface_format.colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;

		ColorSpace pre_colorspace = internal_state->color_space;
		switch (surface_format.colorSpace)
		{
		case VK_COLOR_SPACE_SRGB_NONLINEAR_KHR:
			internal_state->color_space = ColorSpace::SRGB;
			break;
		case VK_COLOR_SPACE_EXTENDED_SRGB_NONLINEAR_EXT:
			internal_state->color_space = ColorSpace::HDR_LINEAR;
			break;
		case VK_COLOR_SPACE_HDR10_ST2084_EXT:
			internal_state->color_space = ColorSpace::HDR10_ST2084;
			break;
		default:
			break;
		}
		if (pre_colorspace != internal_state->color_space)
		{
			if (internal_state->swapchain != VK_NULL_HANDLE)
			{
				result = vkDeviceWaitIdle(rhi->m_device);
				assert(result == VK_SUCCESS);
				vkDestroySwapchainKHR(rhi->m_device, internal_state->swapchain, nullptr);
				internal_state->swapchain = VK_NULL_HANDLE;
			}
		}
		if (swapchain_capabilities.currentExtent.width != 0xFFFFFFFF && swapchain_capabilities.currentExtent.height != 0xFFFFFFFF)
		{
			internal_state->swapChainExtent = swapchain_capabilities.currentExtent;
		}
		else
		{
			internal_state->swapChainExtent = { internal_state->desc.width, internal_state->desc.height };
			internal_state->swapChainExtent.width = std::max(swapchain_capabilities.minImageExtent.width, std::min(swapchain_capabilities.maxImageExtent.width, internal_state->swapChainExtent.width));
			internal_state->swapChainExtent.height = std::max(swapchain_capabilities.minImageExtent.height, std::min(swapchain_capabilities.maxImageExtent.height, internal_state->swapChainExtent.height));
		}

		uint32_t image_count = std::max(internal_state->desc.buffer_count, swapchain_capabilities.minImageCount);
		if (swapchain_capabilities.maxImageCount > 0 && image_count > swapchain_capabilities.maxImageCount)
		{
			image_count = swapchain_capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		create_info.surface = internal_state->surface;
		create_info.minImageCount = image_count;
		create_info.imageFormat = surface_format.format;
		create_info.imageColorSpace = surface_format.colorSpace;
		create_info.imageExtent = internal_state->swapChainExtent;
		create_info.imageArrayLayers = 1;
		create_info.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		create_info.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		create_info.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		create_info.preTransform = swapchain_capabilities.currentTransform;
		create_info.presentMode = VK_PRESENT_MODE_FIFO_KHR;

		//use for vsync
		if (!internal_state->desc.vsync)
		{
			for (auto& present_mode : details.present_modes)
			{
				if (present_mode == VK_PRESENT_MODE_MAILBOX_KHR)
				{
					create_info.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
					break;
				}
				if (present_mode == VK_PRESENT_MODE_IMMEDIATE_KHR)
				{
					create_info.presentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
					break;
				}
			}
		}
		create_info.clipped = VK_TRUE;
		create_info.oldSwapchain = internal_state->swapchain;
		result = vkCreateSwapchainKHR(rhi->m_device, &create_info, nullptr, &internal_state->swapchain);
		assert(result == VK_SUCCESS);

		//TODO:destroy old swapchain

		result = vkGetSwapchainImagesKHR(rhi->m_device, internal_state->swapchain, &image_count, nullptr);
		assert(result == VK_SUCCESS);
		internal_state->swapchain_images.resize(image_count);
		result = vkGetSwapchainImagesKHR(rhi->m_device, internal_state->swapchain, &image_count, internal_state->swapchain_images.data());
		assert(result == VK_SUCCESS);
		internal_state->swapchainImageFormat = surface_format.format;

		//create swap chain render target
		internal_state->swapchain_image_views.resize(internal_state->swapchain_images.size());
		for (int i = 0; i < internal_state->swapchain_image_views.size(); ++i)
		{
			VkImageViewCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			create_info.image = internal_state->swapchain_images[i];
			create_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			create_info.format = internal_state->swapchainImageFormat;
			create_info.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;
			create_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			create_info.subresourceRange.baseMipLevel = 0;
			create_info.subresourceRange.levelCount = 1;
			create_info.subresourceRange.baseArrayLayer = 0;
			create_info.subresourceRange.layerCount = 1;

			if (internal_state->swapchain_image_views[i] != VK_NULL_HANDLE)
			{
				//TODO:destroy old image view

			}
			result = vkCreateImageView(rhi->m_device, &create_info, nullptr, &internal_state->swapchain_image_views[i]);
			assert(result == VK_SUCCESS);
		}

		if (internal_state->swapchainAcquireSemaphores.empty())
		{
			VkSemaphoreCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			internal_state->swapchainAcquireSemaphores.resize(internal_state->swapchain_images.size());
			for (int i = 0; i < internal_state->swapchain_images.size(); ++i)
			{
				result = vkCreateSemaphore(rhi->m_device, &create_info, nullptr, &internal_state->swapchainAcquireSemaphores[i]);
				assert(result == VK_SUCCESS);
			}
		}

		if (internal_state->swapchain_release_semaphore == VK_NULL_HANDLE)
		{
			VkSemaphoreCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			result = vkCreateSemaphore(rhi->m_device, &create_info, nullptr, &internal_state->swapchain_release_semaphore);
			assert(result == VK_SUCCESS);
		}

		return true;
	}

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData) {
		if (messageSeverity == VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
		{
			std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
			int a = 0;
		}

		return VK_FALSE;
	}

	void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
		createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
		createInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
		createInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
		createInfo.pfnUserCallback = debugCallback;
	}

	bool VulkanRHI::createSwapChain(platform::WindowType window, SwapChain* swapchain, SwapChainDesc desc)
	{
		VkResult result;
		std::shared_ptr<SwapChain_Vulkan> swapchain_vulkan = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);
		if (swapchain_vulkan == nullptr)
		{
			swapchain_vulkan = std::make_shared<SwapChain_Vulkan>();
			swapchain->internal_state = swapchain_vulkan;
		}
		swapchain_vulkan->desc = desc;

		//create surface
		if (swapchain_vulkan->surface == VK_NULL_HANDLE)
		{
#ifdef _WIN32
			VkWin32SurfaceCreateInfoKHR surface_info;
			surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surface_info.hwnd = window;
			surface_info.hinstance = GetModuleHandle(nullptr);
			surface_info.pNext = nullptr;
			surface_info.flags = 0;
			result = vkCreateWin32SurfaceKHR(instance, &surface_info, nullptr, &swapchain_vulkan->surface);
			assert(result == VK_SUCCESS);
#endif // _WIN32
			bool success = createInternalSwapChain(this, swapchain_vulkan.get());
			swapchain->desc.format = swapchain_vulkan->desc.format;
			swapchain->desc.width = swapchain_vulkan->desc.width;
			swapchain->desc.height = swapchain_vulkan->desc.height;
		}
		return true;
	}

	void VulkanRHI::createImage(VkPhysicalDevice physical_device,
		VkDevice device, uint32_t width, uint32_t height, VkFormat format,
		uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkImageTiling tiling,
		VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image,
		VkDeviceMemory& imageMemory) {

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mip_levels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = num_samples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physical_device, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(device, image, imageMemory, 0);
	}

	uint32_t VulkanRHI::findMemoryType(VkPhysicalDevice physical_device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	std::vector<const char*> getRequiredExtensions() {
		std::vector<const char*> extensions;
#if defined(USE_GLFW)
		uint32_t glfwExtensionCount = 0;
		const char** glfwExtensions;
		glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
		extensions = std::vector<const char*>(glfwExtensions, glfwExtensions + glfwExtensionCount);
#else
		extensions.push_back(VK_KHR_SURFACE_EXTENSION_NAME);

#endif
#if defined(VK_USE_PLATFORM_WIN32_KHR)
		extensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);

#endif 

		if (enableValidationLayers || enable_debug_utils_label) {
			extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
		}

		return extensions;
	}

	void VulkanRHI::createInstance() {
		if (enableValidationLayers && !checkValidationLayerSupport()) {
			throw std::runtime_error("validation layers requested, but not available!");
		}

		VkApplicationInfo appInfo{};
		appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
		appInfo.pApplicationName = "Hello Triangle";
		appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.pEngineName = "No Engine";
		appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
		appInfo.apiVersion = VK_API_VERSION_1_3;

		VkInstanceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
		createInfo.pApplicationInfo = &appInfo;

		auto extensions = getRequiredExtensions();
		createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
		createInfo.ppEnabledExtensionNames = extensions.data();

		VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo{};
		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();

			populateDebugMessengerCreateInfo(debugCreateInfo);
			createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)&debugCreateInfo;
		}
		else {
			createInfo.enabledLayerCount = 0;

			createInfo.pNext = nullptr;
		}

		if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS) {
			throw std::runtime_error("failed to create instance!");
		}
#ifdef USE_VOLK
		volkLoadInstanceOnly(instance);
#endif // USE_VOLK

	}

	void VulkanRHI::setupDebugMessenger() {
		if (!enableValidationLayers) return;

		VkDebugUtilsMessengerCreateInfoEXT createInfo;
		populateDebugMessengerCreateInfo(createInfo);

		if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
			throw std::runtime_error("failed to set up debug messenger!");
		}
	}

	void VulkanRHI::createSurface(SwapChain* swapchain, HWND hwnd) {
		//use glfw window as engine window
		if (m_window) {
			assert(glfwCreateWindowSurface(instance, m_window, nullptr, &surface) == VK_SUCCESS);
		}
		else
		{
			auto internal_state = std::static_pointer_cast<SwapChain_Vulkan>(swapchain->internal_state);

#ifdef _WIN32 
			//use windows subsystem window 
			VkWin32SurfaceCreateInfoKHR surface_info{};
			surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surface_info.hinstance = GetModuleHandle(nullptr);
			surface_info.hwnd = hwnd;
			VkResult result;
			vkCreateWin32SurfaceKHR(instance, &surface_info, nullptr, &internal_state->surface);

#endif


		}
	}

	void VulkanRHI::bindIndexBuffer(const GPUBuffer* indexBuffer, const IndexBufferFormat format, uint64_t offset, CommandList cmd)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		Buffer_Vulkan* ib = to_internal(indexBuffer);
		vkCmdBindIndexBuffer(commandlist.getCommandBuffer(), ib->resource, offset, format == IndexBufferFormat::UINT16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
	}

	constexpr VkFormat convertFormat(qyhs::graphics::Format value)
	{
		switch (value)
		{
		case Format::UNKNOWN:
			return VK_FORMAT_UNDEFINED;
		case Format::R32G32B32A32_FLOAT:
			return VK_FORMAT_R32G32B32A32_SFLOAT;
		case Format::R32G32B32A32_UINT:
			return VK_FORMAT_R32G32B32A32_UINT;
		case Format::R32G32B32A32_SINT:
			return VK_FORMAT_R32G32B32A32_SINT;
		case Format::R32G32B32_FLOAT:
			return VK_FORMAT_R32G32B32_SFLOAT;
		case Format::R32G32B32_UINT:
			return VK_FORMAT_R32G32B32_UINT;
		case Format::R32G32B32_SINT:
			return VK_FORMAT_R32G32B32_SINT;
		case Format::R16G16B16A16_FLOAT:
			return VK_FORMAT_R16G16B16A16_SFLOAT;
		case Format::R16G16B16A16_UNORM:
			return VK_FORMAT_R16G16B16A16_UNORM;
		case Format::R16G16B16A16_UINT:
			return VK_FORMAT_R16G16B16A16_UINT;
		case Format::R16G16B16A16_SNORM:
			return VK_FORMAT_R16G16B16A16_SNORM;
		case Format::R16G16B16A16_SINT:
			return VK_FORMAT_R16G16B16A16_SINT;
		case Format::R32G32_FLOAT:
			return VK_FORMAT_R32G32_SFLOAT;
		case Format::R32G32_UINT:
			return VK_FORMAT_R32G32_UINT;
		case Format::R32G32_SINT:
			return VK_FORMAT_R32G32_SINT;
		case Format::D32_FLOAT_S8X24_UINT:
			return VK_FORMAT_D32_SFLOAT_S8_UINT;
		case Format::R10G10B10A2_UNORM:
			return VK_FORMAT_A2B10G10R10_UNORM_PACK32;
		case Format::R10G10B10A2_UINT:
			return VK_FORMAT_A2B10G10R10_UINT_PACK32;
		case Format::R11G11B10_FLOAT:
			return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
		case Format::R8G8B8A8_UNORM:
			return VK_FORMAT_R8G8B8A8_UNORM;
		case Format::R8G8B8A8_UNORM_SRGB:
			return VK_FORMAT_R8G8B8A8_SRGB;
		case Format::R8G8B8A8_UINT:
			return VK_FORMAT_R8G8B8A8_UINT;
		case Format::R8G8B8A8_SNORM:
			return VK_FORMAT_R8G8B8A8_SNORM;
		case Format::R8G8B8A8_SINT:
			return VK_FORMAT_R8G8B8A8_SINT;
		case Format::R16G16_FLOAT:
			return VK_FORMAT_R16G16_SFLOAT;
		case Format::R16G16_UNORM:
			return VK_FORMAT_R16G16_UNORM;
		case Format::R16G16_UINT:
			return VK_FORMAT_R16G16_UINT;
		case Format::R16G16_SNORM:
			return VK_FORMAT_R16G16_SNORM;
		case Format::R16G16_SINT:
			return VK_FORMAT_R16G16_SINT;
		case Format::D32_FLOAT:
			return VK_FORMAT_D32_SFLOAT;
		case Format::R32_FLOAT:
			return VK_FORMAT_R32_SFLOAT;
		case Format::R32_UINT:
			return VK_FORMAT_R32_UINT;
		case Format::R32_SINT:
			return VK_FORMAT_R32_SINT;
		case Format::D24_UNORM_S8_UINT:
			return VK_FORMAT_D24_UNORM_S8_UINT;
		case Format::R9G9B9E5_SHAREDEXP:
			return VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;
		case Format::R8G8_UNORM:
			return VK_FORMAT_R8G8_UNORM;
		case Format::R8G8_UINT:
			return VK_FORMAT_R8G8_UINT;
		case Format::R8G8_SNORM:
			return VK_FORMAT_R8G8_SNORM;
		case Format::R8G8_SINT:
			return VK_FORMAT_R8G8_SINT;
		case Format::R16_FLOAT:
			return VK_FORMAT_R16_SFLOAT;
		case Format::D16_UNORM:
			return VK_FORMAT_D16_UNORM;
		case Format::R16_UNORM:
			return VK_FORMAT_R16_UNORM;
		case Format::R16_UINT:
			return VK_FORMAT_R16_UINT;
		case Format::R16_SNORM:
			return VK_FORMAT_R16_SNORM;
		case Format::R16_SINT:
			return VK_FORMAT_R16_SINT;
		case Format::R8_UNORM:
			return VK_FORMAT_R8_UNORM;
		case Format::R8_UINT:
			return VK_FORMAT_R8_UINT;
		case Format::R8_SNORM:
			return VK_FORMAT_R8_SNORM;
		case Format::R8_SINT:
			return VK_FORMAT_R8_SINT;
		case Format::BC1_UNORM:
			return VK_FORMAT_BC1_RGBA_UNORM_BLOCK;
		case Format::BC1_UNORM_SRGB:
			return VK_FORMAT_BC1_RGBA_SRGB_BLOCK;
		case Format::BC2_UNORM:
			return VK_FORMAT_BC2_UNORM_BLOCK;
		case Format::BC2_UNORM_SRGB:
			return VK_FORMAT_BC2_SRGB_BLOCK;
		case Format::BC3_UNORM:
			return VK_FORMAT_BC3_UNORM_BLOCK;
		case Format::BC3_UNORM_SRGB:
			return VK_FORMAT_BC3_SRGB_BLOCK;
		case Format::BC4_UNORM:
			return VK_FORMAT_BC4_UNORM_BLOCK;
		case Format::BC4_SNORM:
			return VK_FORMAT_BC4_SNORM_BLOCK;
		case Format::BC5_UNORM:
			return VK_FORMAT_BC5_UNORM_BLOCK;
		case Format::BC5_SNORM:
			return VK_FORMAT_BC5_SNORM_BLOCK;
		case Format::B8G8R8A8_UNORM:
			return VK_FORMAT_B8G8R8A8_UNORM;
		case Format::B8G8R8A8_UNORM_SRGB:
			return VK_FORMAT_B8G8R8A8_SRGB;
		case Format::BC6H_UF16:
			return VK_FORMAT_BC6H_UFLOAT_BLOCK;
		case Format::BC6H_SF16:
			return VK_FORMAT_BC6H_SFLOAT_BLOCK;
		case Format::BC7_UNORM:
			return VK_FORMAT_BC7_UNORM_BLOCK;
		case Format::BC7_UNORM_SRGB:
			return VK_FORMAT_BC7_SRGB_BLOCK;
		case Format::NV12:
			return VK_FORMAT_G8_B8R8_2PLANE_420_UNORM;
		}
		return VK_FORMAT_UNDEFINED;
	}

	void VulkanRHI::pickPhysicalDevice() {
		uint32_t deviceCount = 0;
		vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

		if (deviceCount == 0) {
			throw std::runtime_error("failed to find GPUs with Vulkan support!");
		}

		std::vector<VkPhysicalDevice> devices(deviceCount);
		vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

		for (const auto& device : devices) {
			if (isDeviceSuitable(device)) {
				physical_device = device;
				//msaa_samples = getMaxAvailableSamplerCount();
				msaa_samples = VK_SAMPLE_COUNT_1_BIT;
				break;
			}
		}

		if (physical_device == VK_NULL_HANDLE) {
			throw std::runtime_error("failed to find a suitable GPU!");
		}

		vkGetPhysicalDeviceProperties2(physical_device, &m_physical_device_properties_2);
		vkGetPhysicalDeviceFeatures2(physical_device, &m_physical_device_features_2);
		assert(m_physical_device_features_1_3.dynamicRendering == VK_TRUE);
	}

	constexpr VkAccessFlags2 convertResourceState(ResourceState value)
	{
		VkAccessFlags2 flags = 0;

		if (has_flag(value, ResourceState::SHADER_RESOURCE))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
		}
		if (has_flag(value, ResourceState::SHADER_RESOURCE_COMPUTE))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
		}
		if (has_flag(value, ResourceState::UNORDERED_ACCESS))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
			flags |= VK_ACCESS_2_SHADER_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::COPY_SRC))
		{
			flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
		}
		if (has_flag(value, ResourceState::COPY_DST))
		{
			flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::RENDERTARGET))
		{
			flags |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			flags |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::DEPTHSTENCIL))
		{
			flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::DEPTHSTENCIL_READONLY))
		{
			flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}
		if (has_flag(value, ResourceState::VERTEX_BUFFER))
		{
			flags |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		}
		if (has_flag(value, ResourceState::INDEX_BUFFER))
		{
			flags |= VK_ACCESS_2_INDEX_READ_BIT;
		}
		if (has_flag(value, ResourceState::CONSTANT_BUFFER))
		{
			flags |= VK_ACCESS_2_UNIFORM_READ_BIT;
		}
		if (has_flag(value, ResourceState::INDIRECT_ARGUMENT))
		{
			flags |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
		}
		if (has_flag(value, ResourceState::PREDICATION))
		{
			flags |= VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT;
		}
		if (has_flag(value, ResourceState::SHADING_RATE_SOURCE))
		{
			flags |= VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
		}
		if (has_flag(value, ResourceState::VIDEO_DECODE_DST))
		{
			flags |= VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR;
		}
		if (has_flag(value, ResourceState::VIDEO_DECODE_SRC))
		{
			flags |= VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR;
		}
		return flags;
	}

	void VulkanRHI::createLogicalDevice() {
		queue_family_indices = findQueueFamilies(physical_device);

		// Find queue families:
		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queueFamilyCount, nullptr);

		queue_families.resize(queueFamilyCount);
		queue_families_video.resize(queueFamilyCount);
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			queue_families[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_PROPERTIES_2;
			queue_families[i].pNext = &queue_families_video[i];
			queue_families_video[i].sType = VK_STRUCTURE_TYPE_QUEUE_FAMILY_VIDEO_PROPERTIES_KHR;
		}
		vkGetPhysicalDeviceQueueFamilyProperties2(physical_device, &queueFamilyCount, queue_families.data());

		// Query base queue families:
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			auto& queueFamily = queue_families[i];
			auto& queueFamilyVideo = queue_families_video[i];

			if (graphics_family == VK_QUEUE_FAMILY_IGNORED && queueFamily.queueFamilyProperties.queueCount > 0 && queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				graphics_family = i;
				if (queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
				{
					queues[QUEUE_GRAPHICS].sparse_binding_supported = true;
				}
			}

			if (transfer_family == VK_QUEUE_FAMILY_IGNORED && queueFamily.queueFamilyProperties.queueCount > 0 && queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				transfer_family = i;
			}

			if (compute_family == VK_QUEUE_FAMILY_IGNORED && queueFamily.queueFamilyProperties.queueCount > 0 && queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				compute_family = i;
			}

			if (video_family == VK_QUEUE_FAMILY_IGNORED &&
				queueFamily.queueFamilyProperties.queueCount > 0 &&
				(queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) &&
				(queueFamilyVideo.videoCodecOperations & VK_VIDEO_CODEC_OPERATION_DECODE_H264_BIT_KHR)
				)
			{
				video_family = i;
			}
		}

		// Now try to find dedicated compute and transfer queues:
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			auto& queueFamily = queue_families[i];

			if (queueFamily.queueFamilyProperties.queueCount > 0 &&
				queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_TRANSFER_BIT &&
				!(queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT) &&
				!(queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT)
				)
			{
				transfer_family = i;

				if (queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
				{
					queues[QUEUE_TRANSFER].sparse_binding_supported = true;
				}
			}

			if (queueFamily.queueFamilyProperties.queueCount > 0 &&
				queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_COMPUTE_BIT &&
				!(queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_GRAPHICS_BIT)
				)
			{
				compute_family = i;

				if (queueFamily.queueFamilyProperties.queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)
				{
					queues[QUEUE_COMPUTE].sparse_binding_supported = true;
				}
			}
		}

		// Now try to find dedicated transfer queue with only transfer and sparse flags:
		//	(This is a workaround for a driver bug with sparse updating from transfer queue)
		for (uint32_t i = 0; i < queueFamilyCount; ++i)
		{
			auto& queueFamily = queue_families[i];

			if (queueFamily.queueFamilyProperties.queueCount > 0 && queueFamily.queueFamilyProperties.queueFlags == (VK_QUEUE_TRANSFER_BIT | VK_QUEUE_SPARSE_BINDING_BIT))
			{
				transfer_family = i;
				queues[QUEUE_TRANSFER].sparse_binding_supported = true;
			}
		}

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::unordered_set<uint32_t> uniqueQueueFamilies = { graphics_family,transfer_family,compute_family };
		if (video_family != VK_QUEUE_FAMILY_IGNORED)
		{
			uniqueQueueFamilies.insert(video_family);
		}

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			queue_lockers.emplace(queueFamily, std::make_shared<std::mutex>());

			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
			families.push_back(queueFamily);
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		deviceFeatures.samplerAnisotropy = VK_TRUE;
		deviceFeatures.sampleRateShading = VK_TRUE;

		VkDeviceCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
		createInfo.pQueueCreateInfos = queueCreateInfos.data();

		createInfo.pEnabledFeatures = nullptr;					//use either pEnabledFeatures or pNext,can't use both
		createInfo.pNext = &m_physical_device_features_2;
		createInfo.enabledExtensionCount = static_cast<uint32_t>(enabled_extensions.size());
		createInfo.ppEnabledExtensionNames = enabled_extensions.data();

		if (enableValidationLayers) {
			createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
			createInfo.ppEnabledLayerNames = validationLayers.data();
		}
		else {
			createInfo.enabledLayerCount = 0;
		}

		if (vkCreateDevice(physical_device, &createInfo, nullptr, &m_device) != VK_SUCCESS) {
			throw std::runtime_error("failed to create logical device!");
		}

#ifdef USE_VOLK
		volkLoadDevice(m_device);
#endif // USE_VOLK

		vkGetDeviceQueue(m_device, queue_family_indices.graphics_family.value(), 0, &graphics_queue);
		vkGetDeviceQueue(m_device, queue_family_indices.compute_family.value(), 0, &compute_queue);
		vkGetDeviceQueue(m_device, queue_family_indices.copy_family.value(), 0, &copy_queue);
		vkGetDeviceQueue(m_device, queue_family_indices.video_family.value(), 0, &video_queue);
		vkGetDeviceQueue(m_device, queue_family_indices.present_family.value(), 0, &present_queue);
		queues[QUEUE_GRAPHICS].queue = graphics_queue;
		queues[QUEUE_GRAPHICS].locker = queue_lockers[queue_family_indices.graphics_family.value()];

		queues[QUEUE_COMPUTE].queue = compute_queue;
		queues[QUEUE_COMPUTE].locker = queue_lockers[queue_family_indices.compute_family.value()];

		queues[QUEUE_TRANSFER].queue = copy_queue;
		queues[QUEUE_TRANSFER].locker = queue_lockers[queue_family_indices.copy_family.value()];
		queues[QUEUE_VIDEO_DECODE].queue = video_queue;
		if (video_family != VK_QUEUE_FAMILY_IGNORED) {
			queues[QUEUE_VIDEO_DECODE].locker = queue_lockers[video_family];
		}

		m_depth_image_format = findDepthFormat();
	}

	void VulkanRHI::createSwapChain() {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(physical_device, surface);

		VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
		VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.present_modes);
		VkExtent2D extent = chooseSwapExtent(swapChainSupport.capabilities);

		uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
		if (swapChainSupport.capabilities.maxImageCount > 0 && imageCount > swapChainSupport.capabilities.maxImageCount) {
			imageCount = swapChainSupport.capabilities.maxImageCount;
		}

		VkSwapchainCreateInfoKHR createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		createInfo.surface = surface;

		createInfo.minImageCount = imageCount;
		createInfo.imageFormat = surfaceFormat.format;
		createInfo.imageColorSpace = surfaceFormat.colorSpace;
		createInfo.imageExtent = extent;
		createInfo.imageArrayLayers = 1;
		createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

		QueueFamilyIndices indices = findQueueFamilies(physical_device);
		uint32_t queueFamilyIndices[] = { indices.graphics_family.value(), indices.present_family.value() };

		if (indices.graphics_family != indices.present_family) {
			createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			createInfo.queueFamilyIndexCount = 2;
			createInfo.pQueueFamilyIndices = queueFamilyIndices;
		}
		else {
			createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
		createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		createInfo.presentMode = presentMode;
		createInfo.clipped = VK_TRUE;

		if (vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &swap_chain) != VK_SUCCESS) {
			throw std::runtime_error("failed to create swap chain!");
		}

		vkGetSwapchainImagesKHR(m_device, swap_chain, &imageCount, nullptr);
		swapChainImages.resize(imageCount);
		vkGetSwapchainImagesKHR(m_device, swap_chain, &imageCount, swapChainImages.data());

		swapChainImageFormat = surfaceFormat.format;
		m_swapchain_extent = extent;

		m_scissor = { {0, 0}, {m_swapchain_extent.width, m_swapchain_extent.height} };
	}

	void VulkanRHI::createImageViews() {
		swapChainImageViews.resize(swapChainImages.size());

		for (uint32_t i = 0; i < swapChainImages.size(); i++) {
			swapChainImageViews[i] = createImageView(swapChainImages[i], swapChainImageFormat, 1, VK_IMAGE_ASPECT_COLOR_BIT);
		}
	}

	static std::vector<char> readFile(const std::string& filename) {
		std::ifstream file(filename, std::ios::ate | std::ios::binary);

		if (!file.is_open()) {
			throw std::runtime_error("failed to open file!");
		}

		size_t fileSize = (size_t)file.tellg();
		std::vector<char> buffer(fileSize);

		file.seekg(0);
		file.read(buffer.data(), fileSize);

		file.close();

		return buffer;
	}

	QueueFamilyIndices VulkanRHI::findQueueFamilies(VkPhysicalDevice device) {
		QueueFamilyIndices indices;

		uint32_t queueFamilyCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

		std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

		int i = 0;
		for (const auto& queueFamily : queueFamilies) {
			if (!indices.graphics_family.has_value() && queueFamily.queueCount > 0
				&& queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
			{
				indices.graphics_family = i;
				indices.present_family = i;
			}

			if (!indices.compute_family.has_value() && queueFamily.queueCount > 0
				&& queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				indices.compute_family = i;
			}

			if (!indices.copy_family.has_value() && queueFamily.queueCount > 0
				&& queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				indices.copy_family = i;
			}

			if (!indices.video_family.has_value() && queueFamily.queueCount > 0
				&& queueFamily.queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)
			{
				indices.video_family = i;
			}

			if (indices.isComplete()) {
				break;
			}

			i++;
		}

		return indices;
	}

	VkResult VulkanRHI::allocatecommandbuffers(VkCommandBufferAllocateInfo* allocate_info, VkCommandBuffer* command_buffer)
	{
		return vkAllocateCommandBuffers(m_device, allocate_info, command_buffer);
	}

	VkResult VulkanRHI::beginCommandBuffer(VkCommandBuffer* command_buffer, VkCommandBufferBeginInfo* begin_info)
	{
		return vkBeginCommandBuffer(*command_buffer, begin_info);
	}

	VkResult VulkanRHI::endCommandBuffer(VkCommandBuffer* command_buffer)
	{
		return vkEndCommandBuffer(*command_buffer);
	}

	VkResult queueSubmit(VkQueue queue, uint32_t submit_count, VkSubmitInfo* submit_info, VkFence fence)
	{
		return  vkQueueSubmit(queue, submit_count, submit_info, fence);
	}

	VkResult VulkanRHI::queueWaitIdle(VkQueue queue)
	{
		return vkQueueWaitIdle(queue);
	}

	void VulkanRHI::freeCommandBuffers(VkCommandPool command_pool, size_t free_count, VkCommandBuffer* p_command_buffer)
	{
		vkFreeCommandBuffers(m_device, command_pool, free_count, p_command_buffer);
	}

	int VulkanRHI::getDescriptorIndex(const GPUResource* resource, SubresourceType type, int subresource) const
	{
		if (resource == nullptr || !resource->isValid())
			return -1;
		switch (type)
		{
		case qyhs::graphics::SubresourceType::SRV:
		{
			if (resource->isBuffer())
			{
				auto buffer = to_internal((const GPUBuffer*)resource);
				if (subresource < 0)
				{
					return buffer->srv.index;
				}
				else
				{
					return buffer->subresources_srv[subresource].index;
				}
			}
			else if (resource->isTexture())
			{
				auto texture = to_internal((const Texture*)resource);
				if (subresource < 0)
				{
					return texture->srv.index;
				}
				else
				{
					return texture->subresources_srv[subresource].index;
				}
			}
		}
		break;
		case qyhs::graphics::SubresourceType::UAV:
			break;
		case qyhs::graphics::SubresourceType::RTV:
			break;
		case qyhs::graphics::SubresourceType::DSV:
			break;
		default:
			break;
		}
		assert(0);
	}

	VkBlendFactor _ConvertBlend(const Blend& blend)
	{
		switch (blend)
		{
		case Blend::ZERO:
			return VK_BLEND_FACTOR_ZERO;
		case Blend::ONE:
			return VK_BLEND_FACTOR_ONE;
		case Blend::SRC_COLOR:
			return VK_BLEND_FACTOR_SRC_COLOR;
		case Blend::INV_SRC_COLOR:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
		case Blend::SRC_ALPHA:
			return VK_BLEND_FACTOR_SRC_ALPHA;
		case Blend::INV_SRC_ALPHA:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
		case Blend::DEST_ALPHA:
			return VK_BLEND_FACTOR_DST_ALPHA;
		case Blend::INV_DEST_ALPHA:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
		case Blend::DEST_COLOR:
			return VK_BLEND_FACTOR_DST_COLOR;
		case Blend::INV_DEST_COLOR:
			return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
		case Blend::SRC_ALPHA_SAT:
			return VK_BLEND_FACTOR_SRC_ALPHA_SATURATE;
		case Blend::BLEND_FACTOR:
			return VK_BLEND_FACTOR_CONSTANT_COLOR;
		case Blend::INV_BLEND_FACTOR:
			return VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR;
			break;
		case Blend::SRC1_COLOR:
			return VK_BLEND_FACTOR_SRC1_COLOR;
		case Blend::INV_SRC1_COLOR:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR;
		case Blend::SRC1_ALPHA:
			return VK_BLEND_FACTOR_SRC1_ALPHA;
		case Blend::INV_SRC1_ALPHA:
			return VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA;
		default:
			return VK_BLEND_FACTOR_ZERO;
		}
	}

	constexpr VkBlendOp _ConvertBlendOp(BlendOp value)
	{
		switch (value)
		{
		case BlendOp::ADD:
			return VK_BLEND_OP_ADD;
		case BlendOp::SUBTRACT:
			return VK_BLEND_OP_SUBTRACT;
		case BlendOp::REV_SUBTRACT:
			return VK_BLEND_OP_REVERSE_SUBTRACT;
		case BlendOp::MIN:
			return VK_BLEND_OP_MIN;
		case BlendOp::MAX:
			return VK_BLEND_OP_MAX;
		default:
			return VK_BLEND_OP_ADD;
		}
	}

	constexpr uint32_t GetFormatStride(Format format)
	{
		switch (format)
		{
		case Format::BC1_UNORM:
		case Format::BC1_UNORM_SRGB:
		case Format::BC4_SNORM:
		case Format::BC4_UNORM:
			return 8u;

		case Format::R32G32B32A32_FLOAT:
		case Format::R32G32B32A32_UINT:
		case Format::R32G32B32A32_SINT:
		case Format::BC2_UNORM:
		case Format::BC2_UNORM_SRGB:
		case Format::BC3_UNORM:
		case Format::BC3_UNORM_SRGB:
		case Format::BC5_SNORM:
		case Format::BC5_UNORM:
		case Format::BC6H_UF16:
		case Format::BC6H_SF16:
		case Format::BC7_UNORM:
		case Format::BC7_UNORM_SRGB:
			return 16u;

		case Format::R32G32B32_FLOAT:
		case Format::R32G32B32_UINT:
		case Format::R32G32B32_SINT:
			return 12u;

		case Format::R16G16B16A16_FLOAT:
		case Format::R16G16B16A16_UNORM:
		case Format::R16G16B16A16_UINT:
		case Format::R16G16B16A16_SNORM:
		case Format::R16G16B16A16_SINT:
			return 8u;

		case Format::R32G32_FLOAT:
		case Format::R32G32_UINT:
		case Format::R32G32_SINT:
		case Format::D32_FLOAT_S8X24_UINT:
			return 8u;

		case Format::R10G10B10A2_UNORM:
		case Format::R10G10B10A2_UINT:
		case Format::R11G11B10_FLOAT:
		case Format::R8G8B8A8_UNORM:
		case Format::R8G8B8A8_UNORM_SRGB:
		case Format::R8G8B8A8_UINT:
		case Format::R8G8B8A8_SNORM:
		case Format::R8G8B8A8_SINT:
		case Format::B8G8R8A8_UNORM:
		case Format::B8G8R8A8_UNORM_SRGB:
		case Format::R16G16_FLOAT:
		case Format::R16G16_UNORM:
		case Format::R16G16_UINT:
		case Format::R16G16_SNORM:
		case Format::R16G16_SINT:
		case Format::D32_FLOAT:
		case Format::R32_FLOAT:
		case Format::R32_UINT:
		case Format::R32_SINT:
		case Format::D24_UNORM_S8_UINT:
		case Format::R9G9B9E5_SHAREDEXP:
			return 4u;

		case Format::R8G8_UNORM:
		case Format::R8G8_UINT:
		case Format::R8G8_SNORM:
		case Format::R8G8_SINT:
		case Format::R16_FLOAT:
		case Format::D16_UNORM:
		case Format::R16_UNORM:
		case Format::R16_UINT:
		case Format::R16_SNORM:
		case Format::R16_SINT:
			return 2u;

		case Format::R8_UNORM:
		case Format::R8_UINT:
		case Format::R8_SNORM:
		case Format::R8_SINT:
			return 1u;


		default:
			assert(0); // didn't catch format!
			return 16u;
		}
	}

	constexpr bool IsFormatStencilSupport(Format format)
	{
		switch (format)
		{
		case Format::D32_FLOAT_S8X24_UINT:
		case Format::D24_UNORM_S8_UINT:
			return true;
		default:
			return false;
		}
	}

	void VulkanRHI::psoValidate(CommandList cmd)
	{
		CommandList_Vulkan& command_list = getCommandList(cmd);
		if (!command_list.dirty_pso)
		{
			return;
		}
		const PipelineState* pso = command_list.active_pso;
		size_t pipeline_hash = command_list.prev_pipeline_hash;
		auto internal_state = to_internal(pso);
		VkPipeline pipeline = VK_NULL_HANDLE;
		auto iter = pipelines_global.find(pipeline_hash);
		if (iter == pipelines_global.end())
		{
			for (auto& x : command_list.pipelines_worker)
			{
				if (pipeline_hash == x.first)
				{
					pipeline = x.second;
					break;
				}
			}
			if (pipeline == VK_NULL_HANDLE)
			{
				VkGraphicsPipelineCreateInfo pipeline_info = internal_state->pipeline_info;
				VkPipelineMultisampleStateCreateInfo multisampling = {};
				multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				multisampling.sampleShadingEnable = VK_FALSE;
				multisampling.rasterizationSamples = (VkSampleCountFlagBits)command_list.renderpass_info.sample_count;
				if (pso->desc.rasterizer_state != nullptr)
				{
					const RasterizerState& desc = *pso->desc.rasterizer_state;
					if (desc.forced_sample_count > 1)
					{
						multisampling.rasterizationSamples = (VkSampleCountFlagBits)desc.forced_sample_count;
					}
				}
				multisampling.minSampleShading = 1.0f;
				VkSampleMask samplemask = internal_state->samplemask;
				samplemask = pso->desc.sample_mask;
				multisampling.pSampleMask = &samplemask;
				if (pso->desc.blend_state != nullptr)
				{
					multisampling.alphaToCoverageEnable = pso->desc.blend_state->alpha_to_coverage_enable ? VK_TRUE : VK_FALSE;
				}
				else
				{
					multisampling.alphaToCoverageEnable = VK_FALSE;
				}
				multisampling.alphaToOneEnable = VK_FALSE;

				pipeline_info.pMultisampleState = &multisampling;

				//blending
				uint32_t num_blend_attacments = 0;
				VkPipelineColorBlendAttachmentState color_blend_attachments[8] = {};
				for (size_t i = 0; i < command_list.renderpass_info.rt_count; ++i)
				{
					size_t attachment_index = 0;
					if (pso->desc.blend_state->independent_blend_enable)
					{
						attachment_index = i;
					}
					const auto& render_target_blend_state = pso->desc.blend_state->render_targets[attachment_index];
					VkPipelineColorBlendAttachmentState& attachment = color_blend_attachments[num_blend_attacments];
					++num_blend_attacments;
					attachment.blendEnable = render_target_blend_state.blend_enable ? VK_TRUE : VK_FALSE;
					attachment.colorWriteMask = 0;
					const auto& desc = pso->desc.blend_state->render_targets[attachment_index];
					if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_RED))
					{
						attachment.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
					}
					if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_GREEN))
					{
						attachment.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
					}
					if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_BLUE))
					{
						attachment.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
					}
					if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_ALPHA))
					{
						attachment.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
					}

					attachment.srcColorBlendFactor = _ConvertBlend(desc.src_blend);
					attachment.dstColorBlendFactor = _ConvertBlend(desc.dest_blend);
					attachment.colorBlendOp = _ConvertBlendOp(desc.blend_op);
					attachment.srcAlphaBlendFactor = _ConvertBlend(desc.src_blend_alpha);
					attachment.dstAlphaBlendFactor = _ConvertBlend(desc.dest_blend_alpha);
					attachment.alphaBlendOp = _ConvertBlendOp(desc.blend_op_alpha);

				}
				VkPipelineColorBlendStateCreateInfo colorBlending = {};
				colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlending.logicOpEnable = VK_FALSE;
				colorBlending.logicOp = VK_LOGIC_OP_COPY;
				colorBlending.attachmentCount = num_blend_attacments;
				colorBlending.pAttachments = color_blend_attachments;
				colorBlending.blendConstants[0] = 1.0f;
				colorBlending.blendConstants[1] = 1.0f;
				colorBlending.blendConstants[2] = 1.0f;
				colorBlending.blendConstants[3] = 1.0f;

				pipeline_info.pColorBlendState = &colorBlending;

				// Input layout:
				VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
				vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				std::vector<VkVertexInputBindingDescription> bindings;
				std::vector<VkVertexInputAttributeDescription> attributes;
				if (pso->desc.input_layout != nullptr)
				{
					uint32_t lastBinding = 0xFFFFFFFF;
					for (auto& x : pso->desc.input_layout->elements)
					{
						if (x.input_slot == lastBinding)
							continue;
						lastBinding = x.input_slot;
						VkVertexInputBindingDescription& bind = bindings.emplace_back();
						bind.binding = x.input_slot;
						bind.inputRate = x.input_slot_class == InputClassification::PER_VERTEX_DATA ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
						bind.stride = GetFormatStride(x.format);
					}

					uint32_t offset = 0;
					uint32_t i = 0;
					lastBinding = 0xFFFFFFFF;
					for (auto& x : pso->desc.input_layout->elements)
					{
						VkVertexInputAttributeDescription attr = {};
						attr.binding = x.input_slot;
						if (attr.binding != lastBinding)
						{
							lastBinding = attr.binding;
							offset = 0;
						}
						attr.format = convertFormat(x.format);
						attr.location = i;
						attr.offset = x.aligned_byte_offset;
						if (attr.offset == InputLayout::APPEND_ALIGNED_ELEMENT)
						{
							// need to manually resolve this from the format spec.
							attr.offset = offset;
							offset += GetFormatStride(x.format);
						}

						attributes.push_back(attr);

						i++;
					}

					vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
					vertexInputInfo.pVertexBindingDescriptions = bindings.data();
					vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
					vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
				}
				pipeline_info.pVertexInputState = &vertexInputInfo;

				pipeline_info.renderPass = VK_NULL_HANDLE;
				VkPipelineRenderingCreateInfo renderingInfo = {};
				renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
				renderingInfo.viewMask = 0;
				renderingInfo.colorAttachmentCount = command_list.renderpass_info.rt_count;
				VkFormat formats[8] = {};
				for (uint32_t i = 0; i < command_list.renderpass_info.rt_count; ++i)
				{
					formats[i] = convertFormat(command_list.renderpass_info.rt_formats[i]);
				}
				renderingInfo.pColorAttachmentFormats = formats;
				renderingInfo.depthAttachmentFormat = convertFormat(command_list.renderpass_info.ds_format);
				if (IsFormatStencilSupport(command_list.renderpass_info.ds_format))
				{
					renderingInfo.stencilAttachmentFormat = renderingInfo.depthAttachmentFormat;
				}
				pipeline_info.pNext = &renderingInfo;
				VkResult res = vkCreateGraphicsPipelines(m_device, pipeline_cache, 1, &pipeline_info, nullptr, &pipeline);
				assert(res == VK_SUCCESS);

				command_list.pipelines_worker.push_back(std::make_pair(pipeline_hash, pipeline));
			}
		}
		else
		{
			pipeline = iter->second;
		}
		assert(pipeline != VK_NULL_HANDLE);

		vkCmdBindPipeline(command_list.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
		command_list.dirty_pso = false;
	}

	void VulkanRHI::predraw(CommandList cmd)
	{
		psoValidate(cmd);
		CommandList_Vulkan& command_list = getCommandList(cmd);
		command_list.binder.flush(true, cmd);
	}

	void VulkanRHI::endEvent(CommandList cmd)
	{
		if (!m_enable_debug_util)
		{
			return;
		}
		CommandList_Vulkan& command_list = getCommandList(cmd);
		_vkCmdEndDebugUtilsLabelEXT(command_list.getCommandBuffer());
	}

	void VulkanRHI::drawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation, CommandList cmd)
	{
		predraw(cmd);
		CommandList_Vulkan& command_list = getCommandList(cmd);
		vkCmdDrawIndexed(command_list.getCommandBuffer(), indexCount, 1, startIndexLocation, baseVertexLocation, 0);
	}

	void VulkanRHI::draw(uint32_t vertex_count, uint32_t start_draw_location, CommandList cmd)
	{
		predraw(cmd);
		CommandList_Vulkan& command_list = getCommandList(cmd);
		vkCmdDraw(command_list.getCommandBuffer(), vertex_count, 1, start_draw_location, 0);
	}

	constexpr VkCompareOp _ConvertComparisonFunc(ComparisonFunc value)
	{
		switch (value)
		{
		case ComparisonFunc::NEVER:
			return VK_COMPARE_OP_NEVER;
		case ComparisonFunc::LESS:
			return VK_COMPARE_OP_LESS;
		case ComparisonFunc::EQUAL:
			return VK_COMPARE_OP_EQUAL;
		case ComparisonFunc::LESS_EQUAL:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case ComparisonFunc::GREATER:
			return VK_COMPARE_OP_GREATER;
		case ComparisonFunc::NOT_EQUAL:
			return VK_COMPARE_OP_NOT_EQUAL;
		case ComparisonFunc::GREATER_EQUAL:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case ComparisonFunc::ALWAYS:
			return VK_COMPARE_OP_ALWAYS;
		default:
			return VK_COMPARE_OP_NEVER;
		}
	}

	constexpr VkStencilOp _ConvertStencilOp(StencilOp value)
	{
		switch (value)
		{
		case graphics::StencilOp::KEEP:
			return VK_STENCIL_OP_KEEP;
		case graphics::StencilOp::ZERO:
			return VK_STENCIL_OP_ZERO;
		case graphics::StencilOp::REPLACE:
			return VK_STENCIL_OP_REPLACE;
		case graphics::StencilOp::INCR_SAT:
			return VK_STENCIL_OP_INCREMENT_AND_CLAMP;
		case graphics::StencilOp::DECR_SAT:
			return VK_STENCIL_OP_DECREMENT_AND_CLAMP;
		case graphics::StencilOp::INVERT:
			return VK_STENCIL_OP_INVERT;
		case graphics::StencilOp::INCR:
			return VK_STENCIL_OP_INCREMENT_AND_WRAP;
		case graphics::StencilOp::DECR:
			return VK_STENCIL_OP_DECREMENT_AND_WRAP;
		default:
			return VK_STENCIL_OP_KEEP;
		}
	}

	bool VulkanRHI::createPipelineState(const PipelineStateDesc* desc, PipelineState* pso, const RenderPassInfo* renderpass_info)
	{
		auto internal_state = std::make_shared<PipelineState_Vulkan>();
		pso->internal_state = internal_state;
		pso->desc = *desc;
		VkResult res = VK_SUCCESS;

		internal_state->hash = 0;
		qyhs::helper::hash_combine(internal_state->hash, desc->mesh_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->amplification_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->vertex_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->fragment_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->hull_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->domain_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->geometry_shader);
		qyhs::helper::hash_combine(internal_state->hash, desc->input_layout);
		qyhs::helper::hash_combine(internal_state->hash, desc->rasterizer_state);
		qyhs::helper::hash_combine(internal_state->hash, desc->blend_state);
		qyhs::helper::hash_combine(internal_state->hash, desc->depth_stencil_state);
		qyhs::helper::hash_combine(internal_state->hash, desc->primitive_topology);
		qyhs::helper::hash_combine(internal_state->hash, desc->sample_mask);

		{
			auto insert_shader = [&](const Shader* shader) {
				if (shader == nullptr)
				{
					return;
				}
				Shader_Vulkan* shader_internal = to_internal(shader);
				size_t i = 0;
				for (VkDescriptorSetLayoutBinding& x : shader_internal->layout_bindings)
				{
					bool found = false;
					for (VkDescriptorSetLayoutBinding& y : internal_state->layout_bindings)
					{
						if (x.binding == y.binding)
						{
							assert(x.descriptor_count == y.descriptor_count);
							assert(x.descriptorType == y.descriptorType);
							found = true;
							y.stageFlags |= x.stageFlags;
							break;
						}
					}
					if (!found)
					{
						internal_state->layout_bindings.push_back(x);
						internal_state->imageViewTypes.push_back(shader_internal->imageViewTypes[i]);
						if (x.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER || x.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
						{
							for (uint32_t k = 0; k < x.descriptor_count; ++k)
							{
								internal_state->uniform_buffer_sizes[x.binding + k] = shader_internal->uniform_buffer_sizes[x.binding + k];
							}
						}
						if (x.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC)
						{
							for (uint32_t k = 0; k < x.descriptor_count; ++k)
							{
								internal_state->uniform_buffer_dynamic_slots.push_back(x.binding + k);
							}
						}


					}
					++i;
				}
				if (shader_internal->pushconstants.size > 0)
				{
					internal_state->pushconstants.offset = std::min(internal_state->pushconstants.offset, shader_internal->pushconstants.offset);
					internal_state->pushconstants.size = std::max(internal_state->pushconstants.size, shader_internal->pushconstants.size);
					internal_state->pushconstants.stageFlags |= shader_internal->pushconstants.stageFlags;
				}
				};

			insert_shader(desc->mesh_shader);
			insert_shader(desc->amplification_shader);
			insert_shader(desc->domain_shader);
			insert_shader(desc->fragment_shader);
			insert_shader(desc->geometry_shader);
			insert_shader(desc->hull_shader);
			insert_shader(desc->vertex_shader);

			std::sort(internal_state->uniform_buffer_dynamic_slots.begin(), internal_state->uniform_buffer_dynamic_slots.end());

			auto insert_shader_bindless = [&](const Shader* shader) {
				if (shader == nullptr)
					return;
				auto shader_internal = to_internal(shader);

				internal_state->bindlessBindings.resize(std::max(internal_state->bindlessBindings.size(), shader_internal->bindingless_bindings.size()));

				int i = 0;
				for (auto& x : shader_internal->bindingless_bindings)
				{
					if (x.used)
					{
						if (internal_state->bindlessBindings[i].binding.descriptorType != x.binding.descriptorType)
						{
							internal_state->bindlessBindings[i] = x;
						}
						else
						{
							internal_state->bindlessBindings[i].binding.stageFlags |= x.binding.stageFlags;
						}
					}
					i++;
				}
				};

			insert_shader_bindless(desc->mesh_shader);
			insert_shader_bindless(desc->amplification_shader);
			insert_shader_bindless(desc->vertex_shader);
			insert_shader_bindless(desc->hull_shader);
			insert_shader_bindless(desc->domain_shader);
			insert_shader_bindless(desc->geometry_shader);
			insert_shader_bindless(desc->fragment_shader);

			internal_state->binding_hash = 0;
			size_t i = 0;
			for (auto& x : internal_state->layout_bindings)
			{
				qyhs::helper::hash_combine(internal_state->binding_hash, x.binding);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.descriptor_count);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.descriptorType);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.stageFlags);
				qyhs::helper::hash_combine(internal_state->binding_hash, internal_state->imageViewTypes[i++]);
			}
			for (auto& x : internal_state->bindlessBindings)
			{
				qyhs::helper::hash_combine(internal_state->binding_hash, x.used);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.binding.binding);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.binding.descriptor_count);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.binding.descriptorType);
				qyhs::helper::hash_combine(internal_state->binding_hash, x.binding.stageFlags);
			}
			qyhs::helper::hash_combine(internal_state->binding_hash, internal_state->pushconstants.offset);
			qyhs::helper::hash_combine(internal_state->binding_hash, internal_state->pushconstants.size);
			qyhs::helper::hash_combine(internal_state->binding_hash, internal_state->pushconstants.stageFlags);


			pso_layout_cache_mutex.lock();
			if (pso_layout_cache[internal_state->binding_hash].pipeline_layout == VK_NULL_HANDLE)
			{
				std::vector<VkDescriptorSetLayout> layouts;
				{
					VkDescriptorSetLayoutCreateInfo descriptorSetlayoutInfo = {};
					descriptorSetlayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
					descriptorSetlayoutInfo.pBindings = internal_state->layout_bindings.data();
					descriptorSetlayoutInfo.bindingCount = static_cast<uint32_t>(internal_state->layout_bindings.size());
					res = vkCreateDescriptorSetLayout(m_device, &descriptorSetlayoutInfo, nullptr, &internal_state->descriptorSetLayout);
					assert(res == VK_SUCCESS);
					layouts.push_back(internal_state->descriptorSetLayout);
				}

				internal_state->bindlessFirstSet = (uint32_t)layouts.size();
				for (auto& x : internal_state->bindlessBindings)
				{
					switch (x.binding.descriptorType)
					{
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
						assert(0); // not supported, use the raw buffers for same functionality
						break;
					case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
						layouts.push_back(allocation_handler->bindless_sampled_images.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_sampled_images.descriptor_set);
						break;
					case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
						layouts.push_back(allocation_handler->bindless_uniform_texel_buffers.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_uniform_texel_buffers.descriptor_set);
						break;
					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
						layouts.push_back(allocation_handler->bindless_storage_buffers.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_storage_buffers.descriptor_set);
						break;
					case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
						layouts.push_back(allocation_handler->bindless_storage_images.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_storage_images.descriptor_set);
						break;
					case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
						layouts.push_back(allocation_handler->bindless_storage_texel_buffers.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_storage_texel_buffers.descriptor_set);
						break;
					case VK_DESCRIPTOR_TYPE_SAMPLER:
						layouts.push_back(allocation_handler->bindless_samplers.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_samplers.descriptor_set);
						break;
					case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
						layouts.push_back(allocation_handler->bindless_acceleration_structures.descriptor_set_layout);
						internal_state->bindlessSets.push_back(allocation_handler->bindless_acceleration_structures.descriptor_set);
						break;
					default:
						break;
					}

				}

				VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
				pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
				pipelineLayoutInfo.pSetLayouts = layouts.data();
				pipelineLayoutInfo.setLayoutCount = (uint32_t)layouts.size();
				if (internal_state->pushconstants.size > 0)
				{
					pipelineLayoutInfo.pushConstantRangeCount = 1;
					pipelineLayoutInfo.pPushConstantRanges = &internal_state->pushconstants;
				}
				else
				{
					pipelineLayoutInfo.pushConstantRangeCount = 0;
					pipelineLayoutInfo.pPushConstantRanges = nullptr;
				}

				res = vkCreatePipelineLayout(m_device, &pipelineLayoutInfo, nullptr, &internal_state->pipeline_layout);
				assert(res == VK_SUCCESS);
				if (res == VK_SUCCESS)
				{
					pso_layout_cache[internal_state->binding_hash].descriptorSetLayout = internal_state->descriptorSetLayout;
					pso_layout_cache[internal_state->binding_hash].pipeline_layout = internal_state->pipeline_layout;
					pso_layout_cache[internal_state->binding_hash].bindlessSets = internal_state->bindlessSets;
					pso_layout_cache[internal_state->binding_hash].bindlessFirstSet = internal_state->bindlessFirstSet;
				}
			}
			else
			{
				internal_state->descriptorSetLayout = pso_layout_cache[internal_state->binding_hash].descriptorSetLayout;
				internal_state->pipeline_layout = pso_layout_cache[internal_state->binding_hash].pipeline_layout;
				internal_state->bindlessSets = pso_layout_cache[internal_state->binding_hash].bindlessSets;
				internal_state->bindlessFirstSet = pso_layout_cache[internal_state->binding_hash].bindlessFirstSet;
			}
			pso_layout_cache_mutex.unlock();
		}

		VkGraphicsPipelineCreateInfo& pipelineInfo = internal_state->pipeline_info;
		//pipelineInfo.flags = VK_PIPELINE_CREATE_DISABLE_OPTIMIZATION_BIT;
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.layout = internal_state->pipeline_layout;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		// Shaders:

		uint32_t shaderStageCount = 0;
		auto& shaderStages = internal_state->shaderStages;
		if (pso->desc.mesh_shader != nullptr && pso->desc.mesh_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.mesh_shader)->stage_info;
		}
		if (pso->desc.amplification_shader != nullptr && pso->desc.amplification_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.amplification_shader)->stage_info;
		}
		if (pso->desc.vertex_shader != nullptr && pso->desc.vertex_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.vertex_shader)->stage_info;
		}
		if (pso->desc.hull_shader != nullptr && pso->desc.hull_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.hull_shader)->stage_info;
		}
		if (pso->desc.domain_shader != nullptr && pso->desc.domain_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.domain_shader)->stage_info;
		}
		if (pso->desc.geometry_shader != nullptr && pso->desc.geometry_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.geometry_shader)->stage_info;
		}
		if (pso->desc.fragment_shader != nullptr && pso->desc.fragment_shader->isValid())
		{
			shaderStages[shaderStageCount++] = to_internal(pso->desc.fragment_shader)->stage_info;
		}
		pipelineInfo.stageCount = shaderStageCount;
		pipelineInfo.pStages = shaderStages;


		// Fixed function states:

		// Primitive type:
		VkPipelineInputAssemblyStateCreateInfo& inputAssembly = internal_state->input_assembly;
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		switch (pso->desc.primitive_topology)
		{
		case PrimitiveTopology::POINT_LIST:
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_POINT_LIST;
			break;
		case PrimitiveTopology::LINE_LIST:
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			break;
		case PrimitiveTopology::LINE_STRIP:
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
			break;
		case PrimitiveTopology::TRIANGLE_STRIP:
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
			break;
		case PrimitiveTopology::TRIANGLE_LIST:
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			break;
		case PrimitiveTopology::PATCH_LIST:
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;
			break;
		default:
			break;
		}
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		pipelineInfo.pInputAssemblyState = &inputAssembly;


		// Rasterizer:
		VkPipelineRasterizationStateCreateInfo& rasterizer = internal_state->rasterizer;
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_TRUE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_NONE;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;
		rasterizer.depthBiasConstantFactor = 0.0f;
		rasterizer.depthBiasClamp = 0.0f;
		rasterizer.depthBiasSlopeFactor = 0.0f;

		const void** tail = &rasterizer.pNext;

		// depth clip will be enabled via Vulkan 1.1 extension VK_EXT_depth_clip_enable:
		VkPipelineRasterizationDepthClipStateCreateInfoEXT& depthClipStateInfo = internal_state->depth_clip_state_info;
		depthClipStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT;
		depthClipStateInfo.depthClipEnable = VK_TRUE;
		if (depth_clip_enable_features.depthClipEnable == VK_TRUE)
		{
			*tail = &depthClipStateInfo;
			tail = &depthClipStateInfo.pNext;
		}

		if (pso->desc.rasterizer_state != nullptr)
		{
			const RasterizerState& desc = *pso->desc.rasterizer_state;

			switch (desc.fill_mode)
			{
			case FillMode::WIREFRAME:
				rasterizer.polygonMode = VK_POLYGON_MODE_LINE;
				break;
			case FillMode::SOLID:
			default:
				rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
				break;
			}

			switch (desc.cull_mode)
			{
			case CullMode::BACK:
				rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
				break;
			case CullMode::FRONT:
				rasterizer.cullMode = VK_CULL_MODE_FRONT_BIT;
				break;
			case CullMode::NONE:
			default:
				rasterizer.cullMode = VK_CULL_MODE_NONE;
				break;
			}

			rasterizer.frontFace = desc.front_counter_clockwise ? VK_FRONT_FACE_COUNTER_CLOCKWISE : VK_FRONT_FACE_CLOCKWISE;
			rasterizer.depthBiasEnable = desc.depth_bias != 0 || desc.slope_scaled_depth_bias != 0;
			rasterizer.depthBiasConstantFactor = static_cast<float>(desc.depth_bias);
			rasterizer.depthBiasClamp = desc.depth_bias_clamp;
			rasterizer.depthBiasSlopeFactor = desc.slope_scaled_depth_bias;

			// Depth clip will be enabled via Vulkan 1.1 extension VK_EXT_depth_clip_enable:
			depthClipStateInfo.depthClipEnable = desc.depth_clip_enable ? VK_TRUE : VK_FALSE;

			VkPipelineRasterizationConservativeStateCreateInfoEXT& rasterizationConservativeState = internal_state->rasterization_conservative_state;
			if (checkCapability(GraphicsDeviceCapability::CONSERVATIVE_RASTERIZATION) && desc.conservative_rasterization_enable)
			{
				rasterizationConservativeState.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_CONSERVATIVE_STATE_CREATE_INFO_EXT;
				rasterizationConservativeState.conservativeRasterizationMode = VK_CONSERVATIVE_RASTERIZATION_MODE_OVERESTIMATE_EXT;
				rasterizationConservativeState.extraPrimitiveOverestimationSize = 0.0f;
				*tail = &rasterizationConservativeState;
				tail = &rasterizationConservativeState.pNext;
			}
		}

		pipelineInfo.pRasterizationState = &rasterizer;

		VkPipelineViewportStateCreateInfo& viewportState = internal_state->viewport_state;
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 0;
		viewportState.pViewports = nullptr;
		viewportState.scissorCount = 0;
		viewportState.pScissors = nullptr;

		pipelineInfo.pViewportState = &viewportState;


		// Depth-Stencil:
		VkPipelineDepthStencilStateCreateInfo& depthstencil = internal_state->depthstencil;
		depthstencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		if (pso->desc.depth_stencil_state != nullptr)
		{
			depthstencil.depthTestEnable = pso->desc.depth_stencil_state->depth_enable ? VK_TRUE : VK_FALSE;
			depthstencil.depthWriteEnable = pso->desc.depth_stencil_state->depth_write_mask == DepthWriteMask::ZERO ? VK_FALSE : VK_TRUE;
			depthstencil.depthCompareOp = _ConvertComparisonFunc(pso->desc.depth_stencil_state->depth_func);

			if (pso->desc.depth_stencil_state->stencil_enable)
			{
				depthstencil.stencilTestEnable = VK_TRUE;

				depthstencil.front.compareMask = pso->desc.depth_stencil_state->stencil_read_mask;
				depthstencil.front.writeMask = pso->desc.depth_stencil_state->stencil_write_mask;
				depthstencil.front.reference = 0; // runtime supplied
				depthstencil.front.compareOp = _ConvertComparisonFunc(pso->desc.depth_stencil_state->front_face.stencil_func);
				depthstencil.front.passOp = _ConvertStencilOp(pso->desc.depth_stencil_state->front_face.stencil_pass_op);
				depthstencil.front.failOp = _ConvertStencilOp(pso->desc.depth_stencil_state->front_face.stencil_fail_op);
				depthstencil.front.depthFailOp = _ConvertStencilOp(pso->desc.depth_stencil_state->front_face.stencil_depth_fail_op);

				depthstencil.back.compareMask = pso->desc.depth_stencil_state->stencil_read_mask;
				depthstencil.back.writeMask = pso->desc.depth_stencil_state->stencil_write_mask;
				depthstencil.back.reference = 0; // runtime supplied
				depthstencil.back.compareOp = _ConvertComparisonFunc(pso->desc.depth_stencil_state->back_face.stencil_func);
				depthstencil.back.passOp = _ConvertStencilOp(pso->desc.depth_stencil_state->back_face.stencil_pass_op);
				depthstencil.back.failOp = _ConvertStencilOp(pso->desc.depth_stencil_state->back_face.stencil_fail_op);
				depthstencil.back.depthFailOp = _ConvertStencilOp(pso->desc.depth_stencil_state->back_face.stencil_depth_fail_op);
			}
			else
			{
				depthstencil.stencilTestEnable = VK_FALSE;

				depthstencil.front.compareMask = 0;
				depthstencil.front.writeMask = 0;
				depthstencil.front.reference = 0;
				depthstencil.front.compareOp = VK_COMPARE_OP_NEVER;
				depthstencil.front.passOp = VK_STENCIL_OP_KEEP;
				depthstencil.front.failOp = VK_STENCIL_OP_KEEP;
				depthstencil.front.depthFailOp = VK_STENCIL_OP_KEEP;

				depthstencil.back.compareMask = 0;
				depthstencil.back.writeMask = 0;
				depthstencil.back.reference = 0; // runtime supplied
				depthstencil.back.compareOp = VK_COMPARE_OP_NEVER;
				depthstencil.back.passOp = VK_STENCIL_OP_KEEP;
				depthstencil.back.failOp = VK_STENCIL_OP_KEEP;
				depthstencil.back.depthFailOp = VK_STENCIL_OP_KEEP;
			}

			if (checkCapability(GraphicsDeviceCapability::DEPTH_BOUNDS_TEST))
			{
				depthstencil.depthBoundsTestEnable = pso->desc.depth_stencil_state->depth_bounds_test_enable ? VK_TRUE : VK_FALSE;
			}
			else
			{
				depthstencil.depthBoundsTestEnable = VK_FALSE;
			}
		}

		pipelineInfo.pDepthStencilState = &depthstencil;


		// Tessellation:
		VkPipelineTessellationStateCreateInfo& tessellationInfo = internal_state->tessellation_info;
		tessellationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
		tessellationInfo.patchControlPoints = desc->patch_control_points;

		pipelineInfo.pTessellationState = &tessellationInfo;

		if (pso->desc.mesh_shader == nullptr)
		{
			pipelineInfo.pDynamicState = &dynamic_state_info;
		}
		else
		{
			pipelineInfo.pDynamicState = &dynamic_state_info_mesh_shader;
		}

		if (renderpass_info != nullptr)
		{
			// MSAA:
			VkPipelineMultisampleStateCreateInfo multisampling = {};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = (VkSampleCountFlagBits)renderpass_info->sample_count;
			if (pso->desc.rasterizer_state != nullptr)
			{
				const RasterizerState& desc = *pso->desc.rasterizer_state;
				if (desc.forced_sample_count > 1)
				{
					multisampling.rasterizationSamples = (VkSampleCountFlagBits)desc.forced_sample_count;
				}
			}
			multisampling.minSampleShading = 1.0f;
			VkSampleMask samplemask = internal_state->samplemask;
			samplemask = pso->desc.sample_mask;
			multisampling.pSampleMask = &samplemask;
			if (pso->desc.blend_state != nullptr)
			{
				multisampling.alphaToCoverageEnable = pso->desc.blend_state->alpha_to_coverage_enable ? VK_TRUE : VK_FALSE;
			}
			else
			{
				multisampling.alphaToCoverageEnable = VK_FALSE;
			}
			multisampling.alphaToOneEnable = VK_FALSE;

			pipelineInfo.pMultisampleState = &multisampling;


			// Blending:
			uint32_t numBlendAttachments = 0;
			VkPipelineColorBlendAttachmentState colorBlendAttachments[8] = {};
			for (size_t i = 0; i < renderpass_info->rt_count; ++i)
			{
				size_t attachmentIndex = 0;
				if (pso->desc.blend_state->independent_blend_enable)
					attachmentIndex = i;

				const auto& desc = pso->desc.blend_state->render_targets[attachmentIndex];
				VkPipelineColorBlendAttachmentState& attachment = colorBlendAttachments[numBlendAttachments];
				numBlendAttachments++;

				attachment.blendEnable = desc.blend_enable ? VK_TRUE : VK_FALSE;

				attachment.colorWriteMask = 0;
				if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_RED))
				{
					attachment.colorWriteMask |= VK_COLOR_COMPONENT_R_BIT;
				}
				if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_GREEN))
				{
					attachment.colorWriteMask |= VK_COLOR_COMPONENT_G_BIT;
				}
				if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_BLUE))
				{
					attachment.colorWriteMask |= VK_COLOR_COMPONENT_B_BIT;
				}
				if (has_flag(desc.render_target_write_mask, ColorWrite::ENABLE_ALPHA))
				{
					attachment.colorWriteMask |= VK_COLOR_COMPONENT_A_BIT;
				}

				attachment.srcColorBlendFactor = _ConvertBlend(desc.src_blend);
				attachment.dstColorBlendFactor = _ConvertBlend(desc.dest_blend);
				attachment.colorBlendOp = _ConvertBlendOp(desc.blend_op);
				attachment.srcAlphaBlendFactor = _ConvertBlend(desc.src_blend_alpha);
				attachment.dstAlphaBlendFactor = _ConvertBlend(desc.dest_blend_alpha);
				attachment.alphaBlendOp = _ConvertBlendOp(desc.blend_op_alpha);
			}

			VkPipelineColorBlendStateCreateInfo colorBlending = {};
			colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			colorBlending.logicOpEnable = VK_FALSE;
			colorBlending.logicOp = VK_LOGIC_OP_COPY;
			colorBlending.attachmentCount = numBlendAttachments;
			colorBlending.pAttachments = colorBlendAttachments;
			colorBlending.blendConstants[0] = 1.0f;
			colorBlending.blendConstants[1] = 1.0f;
			colorBlending.blendConstants[2] = 1.0f;
			colorBlending.blendConstants[3] = 1.0f;

			pipelineInfo.pColorBlendState = &colorBlending;

			// Input layout:
			VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
			vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			std::vector<VkVertexInputBindingDescription> bindings;
			std::vector<VkVertexInputAttributeDescription> attributes;
			if (pso->desc.input_layout != nullptr)
			{
				uint32_t lastBinding = 0xFFFFFFFF;
				for (auto& x : pso->desc.input_layout->elements)
				{
					if (x.input_slot == lastBinding)
						continue;
					lastBinding = x.input_slot;
					VkVertexInputBindingDescription& bind = bindings.emplace_back();
					bind.binding = x.input_slot;
					bind.inputRate = x.input_slot_class == InputClassification::PER_VERTEX_DATA ? VK_VERTEX_INPUT_RATE_VERTEX : VK_VERTEX_INPUT_RATE_INSTANCE;
					bind.stride = GetFormatStride(x.format);
				}

				uint32_t offset = 0;
				uint32_t i = 0;
				lastBinding = 0xFFFFFFFF;
				for (auto& x : pso->desc.input_layout->elements)
				{
					VkVertexInputAttributeDescription attr = {};
					attr.binding = x.input_slot;
					if (attr.binding != lastBinding)
					{
						lastBinding = attr.binding;
						offset = 0;
					}
					attr.format = convertFormat(x.format);
					attr.location = i;
					attr.offset = x.aligned_byte_offset;
					if (attr.offset == InputLayout::APPEND_ALIGNED_ELEMENT)
					{
						// need to manually resolve this from the format spec.
						attr.offset = offset;
						offset += GetFormatStride(x.format);
					}

					attributes.push_back(attr);

					i++;
				}

				vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindings.size());
				vertexInputInfo.pVertexBindingDescriptions = bindings.data();
				vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributes.size());
				vertexInputInfo.pVertexAttributeDescriptions = attributes.data();
			}
			pipelineInfo.pVertexInputState = &vertexInputInfo;

			pipelineInfo.renderPass = VK_NULL_HANDLE; // instead we use VkPipelineRenderingCreateInfo

			VkPipelineRenderingCreateInfo renderingInfo = {};
			renderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
			renderingInfo.viewMask = 0;
			renderingInfo.colorAttachmentCount = renderpass_info->rt_count;
			VkFormat formats[8] = {};
			for (uint32_t i = 0; i < renderpass_info->rt_count; ++i)
			{
				formats[i] = convertFormat(renderpass_info->rt_formats[i]);
			}
			renderingInfo.pColorAttachmentFormats = formats;
			renderingInfo.depthAttachmentFormat = convertFormat(renderpass_info->ds_format);
			if (IsFormatStencilSupport(renderpass_info->ds_format))
			{
				renderingInfo.stencilAttachmentFormat = renderingInfo.depthAttachmentFormat;
			}
			pipelineInfo.pNext = &renderingInfo;

			VkResult res = vkCreateGraphicsPipelines(m_device, pipeline_cache, 1, &pipelineInfo, nullptr, &internal_state->pipeline);
			assert(res == VK_SUCCESS);
		}
		return res == VK_SUCCESS;
	}

	int VulkanRHI::getDescriptorIndex(const Sampler* sampler)
	{
		if (sampler == nullptr || !sampler->isValid())
			return -1;
		Sampler_Vulkan* internal_state = to_internal(sampler);
		return internal_state->index;
	}

	RHI::GPUBufferAllocator& VulkanRHI::getFrameAllocator(CommandList cmd)
	{
		return getCommandList(cmd).frame_allocators[getBufferIndex()];
	}

	VkResult VulkanRHI::queueSubmit(VkQueue queue, uint32_t submit_count, VkSubmitInfo* submit_info, VkFence fence)
	{
		return vkQueueSubmit(queue, submit_count, submit_info, fence);
	}

	VkSampleCountFlagBits VulkanRHI::getMaxAvailableSamplerCount()
	{
		VkPhysicalDeviceProperties properties;
		vkGetPhysicalDeviceProperties(physical_device, &properties);
		VkSampleCountFlags counts = properties.limits.framebufferColorSampleCounts & properties.limits.framebufferDepthSampleCounts;
		if (counts & VK_SAMPLE_COUNT_64_BIT) { return VK_SAMPLE_COUNT_64_BIT; }
		if (counts & VK_SAMPLE_COUNT_32_BIT) { return VK_SAMPLE_COUNT_32_BIT; }
		if (counts & VK_SAMPLE_COUNT_16_BIT) { return VK_SAMPLE_COUNT_16_BIT; }
		if (counts & VK_SAMPLE_COUNT_8_BIT) { return VK_SAMPLE_COUNT_8_BIT; }
		if (counts & VK_SAMPLE_COUNT_4_BIT) { return VK_SAMPLE_COUNT_4_BIT; }
		if (counts & VK_SAMPLE_COUNT_2_BIT) { return VK_SAMPLE_COUNT_2_BIT; }

		return VK_SAMPLE_COUNT_1_BIT;
	}

	void VulkanRHI::createCommandPool() {
		{
			VkCommandPoolCreateInfo poolInfo{};
			poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			poolInfo.queueFamilyIndex = queue_family_indices.graphics_family.value();

			if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &command_pool) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics command pool!");
			}

		}

		{
			VkCommandPoolCreateInfo command_pool_create_info;
			command_pool_create_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			command_pool_create_info.pNext = NULL;
			command_pool_create_info.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			command_pool_create_info.queueFamilyIndex = queue_family_indices.graphics_family.value();

			for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
			{
				if (vkCreateCommandPool(m_device, &command_pool_create_info, NULL, &m_command_pools[i]) != VK_SUCCESS)
				{
					throw std::runtime_error("vk create command pool");
				}
			}
		}
	}

	void VulkanRHI::createDepthResources() {
		VkFormat depthFormat = findDepthFormat();

		createImage(m_swapchain_extent.width, m_swapchain_extent.height, depthFormat, 1, msaa_samples, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, depthImage, depthImageMemory);
		depth_image_view = createImageView(depthImage, depthFormat, 1, VK_IMAGE_ASPECT_DEPTH_BIT);
	}

	void VulkanRHI::recreateSwapChain() {
		int width = 0, height = 0;
		glfwGetFramebufferSize(m_window, &width, &height);
		while (width == 0 || height == 0) {
			glfwGetFramebufferSize(m_window, &width, &height);
			glfwWaitEvents();
		}

		VkResult res_wait_for_fences =
			vkWaitForFences(m_device, MAX_FRAMES_IN_FLIGHT, m_is_frame_in_flight_fences.data(), VK_TRUE, UINT64_MAX);
		if (VK_SUCCESS != res_wait_for_fences)
		{
			LOG_ERROR("_vkWaitForFences failed");
			return;
		}

		vkDeviceWaitIdle(m_device);

		cleanupSwapChain();

		createSwapChain();
		createViewport();
		createImageViews();
		//createColorResource();
		createDepthResources();
	}

	void VulkanRHI::cleanupSwapChain() {
		vkDeviceWaitIdle(m_device);
		vkWaitForFences(m_device, MAX_FRAMES_IN_FLIGHT, m_is_frame_in_flight_fences.data(), VK_TRUE, UINT64_MAX);
		vkDestroyImageView(m_device, depth_image_view, nullptr);
		vkDestroyImage(m_device, depthImage, nullptr);
		vkFreeMemory(m_device, depthImageMemory, nullptr);

		//vkDestroyImageView(m_device, colorImageView, nullptr);
		//vkDestroyImage(m_device, colorImage, nullptr);
		//vkFreeMemory(m_device, colorImageMemory, nullptr);

		for (auto framebuffer : swapChainFramebuffers) {
			vkDestroyFramebuffer(m_device, framebuffer, nullptr);
		}

		for (auto imageView : swapChainImageViews) {
			vkDestroyImageView(m_device, imageView, nullptr);
		}

		vkDestroySwapchainKHR(m_device, swap_chain, nullptr);
	}

	void VulkanRHI::loadModel() {
		tinyobj::attrib_t attrib;
		std::vector<tinyobj::shape_t> shapes;
		std::vector<tinyobj::material_t> materials;
		std::string warn, err;

		if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str())) {
			throw std::runtime_error(warn + err);
		}

		std::unordered_map<MeshVertex, uint32_t> uniqueVertices{};

		for (const auto& shape : shapes) {
			for (const auto& index : shape.mesh.indices) {
				MeshVertex vertex{};

				vertex.pos = {
					attrib.vertices[3 * index.vertex_index + 0],
					attrib.vertices[3 * index.vertex_index + 1],
					attrib.vertices[3 * index.vertex_index + 2]
				};

				vertex.texCoord = {
					attrib.texcoords[2 * index.texcoord_index + 0],
					1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
				};

				//vertex.color = { 1.0f, 1.0f, 1.0f };

				if (uniqueVertices.count(vertex) == 0) {
					uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
					vertices.push_back(vertex);
				}

				indices.push_back(uniqueVertices[vertex]);
			}
		}
	}

	std::shared_ptr<VulkanRHI::SwapChain_Vulkan> to_internal(SwapChain* swapchain)
	{
		return std::static_pointer_cast<VulkanRHI::SwapChain_Vulkan>(swapchain->internal_state);
	}

	void VulkanRHI::drawImage(const graphics::Texture* texture, const qyhs::image::Params& params, CommandList cmd)
	{
		const Sampler* sampler = &image::samplers[SAMPLER_LINEAR_CLAMP];
	}

	void VulkanRHI::submitCommandLists()
	{
		VkResult result;
		//submit current frame
		{
			uint32_t cmd_last = cmd_count;
			cmd_count = 0;
			for (int i = 0; i < cmd_last; ++i)
			{
				CommandList_Vulkan& command_list = *commandlists[i].get();
				result = vkEndCommandBuffer(command_list.getCommandBuffer());
				assert(result == VK_SUCCESS);

				CommandQueue& queue = queues[command_list.queue];
				const bool dependency = !command_list.signals.empty() || !command_list.waits.empty() || !command_list.wait_queues.empty();
				if (dependency)
				{
					//if current queue is dependent on other queue, we need to wait for the other queue to finish
					queue.submit(this, nullptr);
				}
				VkCommandBufferSubmitInfo& cmd = queue.submit_cmds.emplace_back();
				cmd.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;
				cmd.commandBuffer = command_list.getCommandBuffer();

				for (auto& swapchain : command_list.prev_swapchains)
				{
					std::shared_ptr<SwapChain_Vulkan> internal_state = to_internal(&swapchain);
					queue.submit_swapchains.push_back(internal_state->swapchain);
					queue.submit_swapchain_image_indices.push_back(internal_state->swapchain_image_index);

					VkSemaphoreSubmitInfo& waitSemaphore = queue.submit_waitSemaphoreInfos.emplace_back();
					waitSemaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
					waitSemaphore.semaphore = internal_state->swapchainAcquireSemaphores[internal_state->swapchain_acquire_semaphore_index];
					waitSemaphore.value = 0; // not a timeline semaphore
					waitSemaphore.stageMask = VK_PIPELINE_STAGE_2_COLOR_ATTACHMENT_OUTPUT_BIT;

					queue.submit_signalSemaphores.push_back(internal_state->swapchain_release_semaphore);
					VkSemaphoreSubmitInfo& signalSemaphore = queue.submit_signalSemaphoreInfos.emplace_back();
					signalSemaphore.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
					signalSemaphore.semaphore = internal_state->swapchain_release_semaphore;
					signalSemaphore.value = 0; // not a timeline semaphore
				}

				if (dependency)
				{
					for (auto& wait : command_list.wait_queues)
					{
						CommandQueue& wait_queue = queues[wait.first];
						VkSemaphore& semaphore = wait.second;

						wait_queue.signal(semaphore);
						wait_queue.submit(this, VK_NULL_HANDLE);

						queue.wait(semaphore);
						//free_semaphore(semaphore);
					}
					command_list.wait_queues.clear();
					for (auto& semaphore : command_list.waits)
					{
						queue.wait(semaphore);
					}
					command_list.waits.clear();
					for (auto& semaphore : command_list.signals)
					{
						queue.signal(semaphore);
						//free_semaphore(semaphore);
					}
					command_list.signals.clear();
					queue.submit(this, VK_NULL_HANDLE);
				}

			}
			for (int q = 0; q < QUEUE_COUNT; ++q)
			{
				queues[q].submit(this, frame_fences[getBufferIndex()][q]);
			}

			// From here, we begin a new frame, this affects GetBufferIndex()!
			++frame_count;

			// Initiate stalling CPU when GPU is not yet finished with next frame:
			if (frame_count >= BUFFER_COUNT)
			{
				const uint32_t bufferindex = getBufferIndex();
				for (int queue = 0; queue < QUEUE_COUNT; ++queue)
				{
					if (frame_fences[bufferindex][queue] == VK_NULL_HANDLE)
						continue;

					result = vkWaitForFences(m_device, 1, &frame_fences[bufferindex][queue], VK_TRUE, 0xFFFFFFFFFFFFFFFF);
					assert(result == VK_SUCCESS);

					result = vkResetFences(m_device, 1, &frame_fences[bufferindex][queue]);
					assert(result == VK_SUCCESS);
				}
			}
		}
		allocation_handler->update(frame_count, BUFFER_COUNT);
	}



	void VulkanRHI::prepareBeforeRender(std::function<void()> update_pass_after_recreate_swap_chain)
	{
		waitForFence();
		resetCommandPool();
		getNextImage(update_pass_after_recreate_swap_chain);
		resetFence();
		beginCommandBuffer();
	}

	void VulkanRHI::beginEvent(VkCommandBuffer command_buffer, std::string event_name, std::array<float, 4> color)
	{
		if (!m_enable_debug_util) return;
		VkDebugUtilsLabelEXT label_info;
		label_info.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_LABEL_EXT;
		label_info.pNext = nullptr;
		label_info.pLabelName = event_name.c_str();
		for (int i = 0; i < 4; ++i)
			label_info.color[i] = color[i];
		_vkCmdBeginDebugUtilsLabelEXT(command_buffer, &label_info);
	}



	void VulkanRHI::beginEvent(const std::string& name, CommandList cmd)
	{
		CommandList_Vulkan& command_list = getCommandList(cmd);
		beginEvent(command_list.getCommandBuffer(), name);
	}

	void VulkanRHI::endEvent(VkCommandBuffer command_buffer)
	{
		if (!m_enable_debug_util) return;
		_vkCmdEndDebugUtilsLabelEXT(command_buffer);
	}

	void VulkanRHI::beginCommandBuffer()
	{
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;
		VkCommandBuffer commandBuffer = m_command_buffers[m_current_frame_index];

		if (vkBeginCommandBuffer(m_command_buffers[m_current_frame_index], &beginInfo) != VK_SUCCESS) {
			throw std::runtime_error("failed to begin recording command buffer!");
		}
	}

	void VulkanRHI::createViewport()
	{
		m_viewport.x = 0.0f;
		m_viewport.y = 0.0f;
		m_viewport.width = (float)getSwapChainExtent().width;
		m_viewport.height = (float)getSwapChainExtent().height;
		m_viewport.minDepth = 0.0f;
		m_viewport.maxDepth = 1.0f;
	}

	void VulkanRHI::bindViewports(const CommandList& cmd_list, uint32_t viewport_count, Viewport* viewport)
	{
		VkViewport p_viewport[16];
		assert(viewport_count < ARRAYSIZE(p_viewport));
		assert(viewport_count < m_physical_device_properties_2.properties.limits.maxViewports);
		for (int i = 0; i < viewport_count; ++i)
		{
			p_viewport[i].x = viewport[i].top_left_x;
			p_viewport[i].y = viewport[i].top_left_y + viewport[i].height;
			p_viewport[i].width = viewport[i].width > 1.0 ? viewport[i].width : 1.0;
			p_viewport[i].height = -viewport[i].height;
			p_viewport[i].minDepth = viewport[i].min_depth;
			p_viewport[i].maxDepth = viewport[i].max_depth;

		}
		CommandList_Vulkan& command_list = getCommandList(cmd_list);
		vkCmdSetViewportWithCount(command_list.getCommandBuffer(), viewport_count, p_viewport);
	}

	void VulkanRHI::beginRenderPass(SwapChain* swapchain, CommandList& cmd_list)
	{
		VkResult result;
		CommandList_Vulkan& commandlist = getCommandList(cmd_list);
		std::shared_ptr<SwapChain_Vulkan> internal_swapchain = to_internal(swapchain);

		internal_swapchain->swapchain_acquire_semaphore_index = (internal_swapchain->swapchain_acquire_semaphore_index + 1) % internal_swapchain->swapchainAcquireSemaphores.size();
		internal_swapchain->locker.lock();
		result = vkAcquireNextImageKHR(m_device, internal_swapchain->swapchain, UINT64_MAX, internal_swapchain->swapchainAcquireSemaphores[internal_swapchain->swapchain_acquire_semaphore_index], VK_NULL_HANDLE, &internal_swapchain->swapchain_image_index);
		assert(result == VK_SUCCESS);
		internal_swapchain->locker.unlock();

		commandlist.prev_swapchains.push_back(*swapchain);
		commandlist.renderpass_barriers_end.clear();
		VkRenderingInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		info.renderArea.offset.x = 0;
		info.renderArea.offset.y = 0;
		info.renderArea.extent.width = std::min(swapchain->desc.width, internal_swapchain->swapChainExtent.width);
		info.renderArea.extent.height = std::min(swapchain->desc.height, internal_swapchain->swapChainExtent.height);
		info.layerCount = 1;
		info.pNext = nullptr;

		VkRenderingAttachmentInfo color_attachment = {};
		color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
		color_attachment.imageView = internal_swapchain->swapchain_image_views[internal_swapchain->swapchain_image_index];
		color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		color_attachment.clearValue.color.float32[0] = swapchain->desc.clear_color[0];
		color_attachment.clearValue.color.float32[1] = swapchain->desc.clear_color[1];
		color_attachment.clearValue.color.float32[2] = swapchain->desc.clear_color[2];
		color_attachment.clearValue.color.float32[3] = swapchain->desc.clear_color[3];

		info.colorAttachmentCount = 1;
		info.pColorAttachments = &color_attachment;

		VkImageMemoryBarrier2 barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
		barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;  // 或当前布局
		barrier.newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.image = internal_swapchain->swapchain_images[internal_swapchain->swapchain_image_index];
		barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		barrier.srcAccessMask = VK_ACCESS_2_NONE;
		barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_GRAPHICS_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.levelCount = 1;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

		VkDependencyInfo dependency_info = {};
		dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
		dependency_info.imageMemoryBarrierCount = 1;
		dependency_info.pImageMemoryBarriers = &barrier;

		vkCmdPipelineBarrier2(commandlist.getCommandBuffer(), &dependency_info);

		barrier.oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
		barrier.srcAccessMask = VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_2_NONE;
		commandlist.renderpass_barriers_end.push_back(barrier);

		assert(internal_swapchain->swapchain_image_views[internal_swapchain->swapchain_image_index] != VK_NULL_HANDLE);

		vkCmdBeginRendering(commandlist.getCommandBuffer(), &info);
		commandlist.renderpass_info = RenderPassInfo::from(swapchain->desc);
	}

	void VulkanRHI::beginRenderPass(const RenderPassImage* images, uint32_t image_count, CommandList cmd, RenderPassFlags flags)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		VkRenderingInfo info = {};
		info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		if (has_flag(flags, RenderPassFlags::SUSPENDING))
		{
			info.flags |= VK_RENDERING_SUSPENDING_BIT;
		}
		if (has_flag(flags, RenderPassFlags::RESUMING))
		{
			info.flags |= VK_RENDERING_RESUMING_BIT;
		}
		info.layerCount = 1;
		info.renderArea.offset.x = 0;
		info.renderArea.offset.y = 0;
		if (image_count == 0)
		{
			// no attachments can still render (UAV only rendering)
			info.renderArea.extent.width = m_physical_device_properties_2.properties.limits.maxFramebufferWidth;
			info.renderArea.extent.height = m_physical_device_properties_2.properties.limits.maxFramebufferHeight;
		}
		VkRenderingAttachmentInfo color_attachments[8] = {};
		VkRenderingAttachmentInfo depth_attachment = {};
		VkRenderingAttachmentInfo stencil_attachment = {};
		VkRenderingFragmentShadingRateAttachmentInfoKHR shading_rate_attachment = {};
		bool color = false;
		bool depth = false;
		bool stencil = false;
		uint32_t color_resolve_count = 0;
		for (int i = 0; i < image_count; ++i)
		{
			const RenderPassImage& image = images[i];
			const TextureDesc& desc = image.texture->desc;
			Texture_Vulkan* internal_state = to_internal(image.texture);
			info.renderArea.extent.width = std::max(info.renderArea.extent.width, desc.width);
			info.renderArea.extent.height = std::max(info.renderArea.extent.height, desc.height);
			int subresource = image.subresource;
			Texture_Vulkan::TextureSubresource descriptor;
			VkAttachmentLoadOp loadOp;
			switch (image.loadop)
			{
			default:
			case RenderPassImage::LoadOp::LOAD:
				loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
				break;
			case RenderPassImage::LoadOp::CLEAR:
				loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
				break;
			case RenderPassImage::LoadOp::DONT_CARE:
				loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
				break;
			}

			VkAttachmentStoreOp storeOp;
			switch (image.storeop)
			{
			default:
			case RenderPassImage::StoreOp::STORE:
				storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				break;
			case RenderPassImage::StoreOp::DONT_CARE:
				storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
				break;
			}
			switch (image.type)
			{
			case RenderPassImage::Type::RENDERTARGET:
			{

				descriptor = subresource < 0 ? internal_state->rtv : internal_state->subresources_rtv[subresource];
				VkRenderingAttachmentInfo& color_attachment = color_attachments[info.colorAttachmentCount++];
				color_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
				color_attachment.imageView = descriptor.image_view;
				color_attachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				color_attachment.loadOp = loadOp;
				color_attachment.storeOp = storeOp;
				color_attachment.clearValue.color.float32[0] = desc.clear.color[0];
				color_attachment.clearValue.color.float32[1] = desc.clear.color[1];
				color_attachment.clearValue.color.float32[2] = desc.clear.color[2];
				color_attachment.clearValue.color.float32[3] = desc.clear.color[3];
				color = true;
			}
			break;

			case RenderPassImage::Type::RESOLVE:
			{
				descriptor = subresource < 0 ? internal_state->srv : internal_state->subresources_srv[subresource];
				VkRenderingAttachmentInfo& color_attachment = color_attachments[color_resolve_count++];
				color_attachment.resolveImageView = descriptor.image_view;
				color_attachment.resolveImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
				color_attachment.resolveMode = VK_RESOLVE_MODE_AVERAGE_BIT;
			}
			break;
			case RenderPassImage::Type::DEPTH_STENCIL:
			{
				descriptor = subresource < 0 ? internal_state->dsv : internal_state->subresources_rtv[subresource];
				depth_attachment.imageView = descriptor.image_view;
				depth_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
				if (image.layout == ResourceState::DEPTHSTENCIL_READONLY)
				{
					depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
				}
				else
				{
					depth_attachment.imageLayout = VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
				}
				depth_attachment.loadOp = loadOp;
				depth_attachment.storeOp = storeOp;
				depth_attachment.clearValue.depthStencil.depth = desc.clear.depth_stencil.depth;
				depth = true;
				if (IsFormatStencilSupport(desc.format))
				{
					stencil_attachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
					stencil_attachment.imageView = subresource < 0 ? internal_state->dsv.image_view : internal_state->subresources_dsv[subresource].image_view;
					if (image.layout == ResourceState::DEPTHSTENCIL_READONLY)
					{
						stencil_attachment.imageLayout = VK_IMAGE_LAYOUT_STENCIL_READ_ONLY_OPTIMAL;
					}
					else
					{
						stencil_attachment.imageLayout = VK_IMAGE_LAYOUT_STENCIL_ATTACHMENT_OPTIMAL;
					}
					stencil_attachment.loadOp = loadOp;
					stencil_attachment.storeOp = storeOp;
					stencil_attachment.clearValue.depthStencil.stencil = desc.clear.depth_stencil.stencil;
					stencil = true;
				}
			}
			break;
			default:
				break;
			}
		}

		info.pColorAttachments = color ? color_attachments : nullptr;
		info.pDepthAttachment = depth ? &depth_attachment : nullptr;
		info.pStencilAttachment = stencil ? &stencil_attachment : nullptr;

		vkCmdBeginRendering(commandlist.getCommandBuffer(), &info);
		commandlist.renderpass_info = RenderPassInfo::from(images, image_count);
	}

	void VulkanRHI::endRenderPass(CommandList cmd_list)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd_list);
		vkCmdEndRendering(commandlist.getCommandBuffer());

		if (!commandlist.renderpass_barriers_end.empty())
		{
			VkDependencyInfo dependency_info = {};
			dependency_info.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
			dependency_info.imageMemoryBarrierCount = static_cast<uint32_t>(commandlist.renderpass_barriers_end.size());
			dependency_info.pImageMemoryBarriers = commandlist.renderpass_barriers_end.data();

			vkCmdPipelineBarrier2(commandlist.getCommandBuffer(), &dependency_info);
			commandlist.renderpass_barriers_end.clear();
		}
		commandlist.renderpass_info = {};
		commandlist.prev_pipeline_hash = 0;
	}

	void VulkanRHI::bindStencilRef(uint32_t stencil_ref, CommandList cmd)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		vkCmdSetStencilReference(commandlist.getCommandBuffer(), VK_STENCIL_FRONT_AND_BACK, stencil_ref);
	}

	bool VulkanRHI::createShader(ShaderStage stage, const void* shader_code, size_t shader_size, Shader* shader)
	{
		std::shared_ptr<Shader_Vulkan > internal_state = std::make_shared<Shader_Vulkan>();
		shader->internal_state = internal_state;
		shader->stage = stage;

		VkShaderModuleCreateInfo module_info{};
		module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		module_info.codeSize = shader_size;
		module_info.pCode = (const uint32_t*)shader_code;
		VkResult result;
		result = vkCreateShaderModule(m_device, &module_info, nullptr, &internal_state->shader_module);
		assert(result == VK_SUCCESS);

		internal_state->stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		internal_state->stage_info.module = internal_state->shader_module;
		internal_state->stage_info.pName = "main";
		switch (stage)
		{
		case ShaderStage::MESH_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_MESH_BIT_EXT;
			break;
		case ShaderStage::AMPLIFICATION_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_TASK_BIT_EXT;
			break;
		case ShaderStage::VERTEX_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case ShaderStage::HULL_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		case ShaderStage::DOMAIN_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			break;
		case ShaderStage::GEOMETRY_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case ShaderStage::PIXEL_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case ShaderStage::COMPUTE_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		default:
			// also means library shader (ray tracing)
			internal_state->stage_info.stage = VK_SHADER_STAGE_ALL;
			break;
		}

		{
			SpvReflectShaderModule module;
			SpvReflectResult result = spvReflectCreateShaderModule(module_info.codeSize, module_info.pCode, &module);
			assert(result == SPV_REFLECT_RESULT_SUCCESS);

			uint32_t binding_count = 0;
			result = spvReflectEnumerateDescriptorBindings(&module, &binding_count, nullptr);
			assert(result == SPV_REFLECT_RESULT_SUCCESS);

			std::vector<SpvReflectDescriptorBinding*> bindings(binding_count);
			result = spvReflectEnumerateDescriptorBindings(&module, &binding_count, bindings.data());
			uint32_t push_count = 0;
			result = spvReflectEnumeratePushConstantBlocks(&module, &push_count, nullptr);
			assert(result == SPV_REFLECT_RESULT_SUCCESS);
			std::vector<SpvReflectBlockVariable*> pushconstants(push_count);
			result = spvReflectEnumeratePushConstantBlocks(&module, &push_count, pushconstants.data());
			assert(result == SPV_REFLECT_RESULT_SUCCESS);

			for (auto& x : pushconstants)
			{
				auto& push = internal_state->pushconstants;
				push.stageFlags = internal_state->stage_info.stage;
				push.offset = x->offset;
				push.size = x->size;
			}

			for (auto& x : bindings)
			{
				//set = 0 for common resource,set>0 for bindless resource
				const bool bindless = x->set > 0;
				size_t bindless_index = x->set - 1;
				if (bindless)
				{
					if (internal_state->bindingless_bindings.size() < x->set)
					{
						internal_state->bindingless_bindings.resize(x->set);
					}
					internal_state->bindingless_bindings[bindless_index].used = true;
				}
				VkDescriptorSetLayoutBinding& descriptor = bindless ? internal_state->bindingless_bindings[bindless_index].binding : internal_state->layout_bindings.emplace_back();
				descriptor.stageFlags = internal_state->stage_info.stage;
				descriptor.descriptorType = (VkDescriptorType)x->descriptor_type;
				descriptor.descriptor_count = x->count;
				descriptor.binding = x->binding;
				descriptor.descriptorType = (VkDescriptorType)x->descriptor_type;
				if (bindless)
				{
					continue;
				}

				auto& imageview_type = internal_state->imageViewTypes.emplace_back();
				imageview_type = VK_IMAGE_VIEW_TYPE_MAX_ENUM;

				if (x->descriptor_type == SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLER && x->binding >= VULKAN_BINDING_SHIFT_S + immutable_sampler_slot_begin)
				{
					descriptor.pImmutableSamplers = immutable_samplers.data() + x->binding - VULKAN_BINDING_SHIFT_S - immutable_sampler_slot_begin;
					continue;
				}

				if (descriptor.descriptorType == VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER)
				{
					descriptor.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
					for (uint32_t i = 0; i < descriptor.descriptor_count; ++i)
					{
						internal_state->uniform_buffer_sizes[descriptor.binding + i] = x->block.size;
						internal_state->uniform_buffer_dynamic_slots.push_back(descriptor.binding + i);
					}
				}

				switch (x->descriptor_type)
				{
				default:
					break;
				case SPV_REFLECT_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
				case SPV_REFLECT_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					switch (x->image.dim)
					{
					default:
					case SpvDim1D:
						if (x->image.arrayed == 0)
						{
							imageview_type = VK_IMAGE_VIEW_TYPE_1D;
						}
						else
						{
							imageview_type = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
						}
						break;
					case SpvDim2D:
						if (x->image.arrayed == 0)
						{
							imageview_type = VK_IMAGE_VIEW_TYPE_2D;
						}
						else
						{
							imageview_type = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
						}
						break;
					case SpvDim3D:
						imageview_type = VK_IMAGE_VIEW_TYPE_3D;
						break;
					case SpvDimCube:
						if (x->image.arrayed == 0)
						{
							imageview_type = VK_IMAGE_VIEW_TYPE_CUBE;
						}
						else
						{
							imageview_type = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
						}
						break;
					}
					break;
				}
			}
			spvReflectDestroyShaderModule(&module);

			assert(result == SPV_REFLECT_RESULT_SUCCESS);
		}


		internal_state->stage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		internal_state->stage_info.module = internal_state->shader_module;
		internal_state->stage_info.pName = "main";

		switch (shader->stage)
		{
		case ShaderStage::MESH_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_MESH_BIT_EXT;
			break;
		case ShaderStage::AMPLIFICATION_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_TASK_BIT_EXT;
			break;
		case ShaderStage::VERTEX_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
			break;
		case ShaderStage::HULL_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
			break;
		case ShaderStage::DOMAIN_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
			break;
		case ShaderStage::GEOMETRY_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_GEOMETRY_BIT;
			break;
		case ShaderStage::PIXEL_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			break;
		case ShaderStage::COMPUTE_SHADER:
			internal_state->stage_info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
			break;
		default:
			// also means library shader (ray tracing)
			internal_state->stage_info.stage = VK_SHADER_STAGE_ALL;
			break;
		}

		return result == VK_SUCCESS;
	}

	void VulkanRHI::bindConstantBuffer(const GPUBuffer* buffer, uint32_t slot, CommandList cmd, uint32_t offset)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		assert(slot < DESCRIPTORBINDER_CBV_COUNT);
		auto& binder = commandlist.binder;

		if (binder.table.CONSTANT_BUFFER_VIEW[slot].internal_state != buffer->internal_state)
		{
			binder.table.CONSTANT_BUFFER_VIEW[slot] = *buffer;
			binder.dirty |= DescriptorBinder::DIRTY_DESCRIPTOR;
		}

		if (binder.table.CBV_offset[slot] != offset)
		{
			binder.table.CBV_offset[slot] = offset;
			binder.dirty |= DescriptorBinder::DIRTY_OFFSET;
		}
	}

	void VulkanRHI::pushConstants(const void* data, uint32_t size, CommandList cmd, uint32_t offset)
	{
		CommandList_Vulkan& command_list = getCommandList(cmd);
		if (command_list.active_pso != nullptr)
		{
			auto pso = to_internal(command_list.active_pso);
			if (pso->pushconstants.size > 0)
			{
				vkCmdPushConstants(command_list.getCommandBuffer(), pso->pipeline_layout, pso->pushconstants.stageFlags, offset, size, data);
				return;
			}
			//assert(0);
		}
	
	}

	void VulkanRHI::drawIndexedInstanced(uint32_t index_count, uint32_t instance_count, uint32_t start_index_location, int32_t base_vertex_location, uint32_t start_instance_location, CommandList cmd)
	{
		predraw(cmd);
		CommandList_Vulkan& command_list = getCommandList(cmd);
		vkCmdDrawIndexed(command_list.getCommandBuffer(), index_count, instance_count, start_index_location, base_vertex_location, start_instance_location);
	}

	void VulkanRHI::copyBuffer(const GPUBuffer* dst_buffer, uint64_t dst_offset, const GPUBuffer* src_buffer, uint64_t src_offset, uint64_t size, CommandList cmd)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		Buffer_Vulkan* internal_state_dst = to_internal(dst_buffer);
		Buffer_Vulkan* internal_state_src = to_internal(src_buffer);
		VkBufferCopy copy = {};
		copy.srcOffset = src_offset;
		copy.dstOffset = dst_offset;
		copy.size = size;
		vkCmdCopyBuffer(commandlist.getCommandBuffer(), internal_state_src->resource, internal_state_dst->resource, 1, &copy);
	}

	void VulkanRHI::submitRender(std::function<void()> update_pass_after_recreate_swap_chain)
	{

		if (vkEndCommandBuffer(m_command_buffers[m_current_frame_index]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		std::vector<VkSemaphore> waitSemaphores = { imageAvailableSemaphores[m_current_frame_index] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = waitSemaphores.size();
		submitInfo.pWaitSemaphores = waitSemaphores.data();;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_command_buffers[m_current_frame_index];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[m_current_frame_index] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		std::lock_guard<std::mutex> lock(queue_submit_mutex);
		if (vkQueueSubmit(graphics_queue, 1, &submitInfo, m_is_frame_in_flight_fences[m_current_frame_index]) != VK_SUCCESS) {
			throw std::runtime_error("failed to submit draw command buffer!");
		}

		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = signalSemaphores;

		VkSwapchainKHR swapChains[] = { swap_chain };
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = swapChains;

		presentInfo.pImageIndices = &current_image_index;

		VkResult result = vkQueuePresentKHR(present_queue, &presentInfo);

		if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR) {

			recreateSwapChain();
			g_runtime_global_context.updateWindow();
			createViewport();
			update_pass_after_recreate_swap_chain();

		}
		else if (result != VK_SUCCESS) {
			throw std::runtime_error("failed to present swap chain image!");
		}

		m_current_frame_index = (m_current_frame_index + 1) % MAX_FRAMES_IN_FLIGHT;

		float time = glfwGetTime();
		delta_time = time - current_time;
		current_time = time;
	}

	void VulkanRHI::destroyImage(VkImage image)
	{
		vkDestroyImage(m_device, image, nullptr);
	}

	void VulkanRHI::destroyImageView(VkImageView image_view)
	{
		vkDestroyImageView(m_device, image_view, nullptr);
	}

	void VulkanRHI::freeMemory(VkDeviceMemory memory)
	{
		vkFreeMemory(m_device, memory, nullptr);
	}

	void VulkanRHI::destroyFrameBuffer(VkFramebuffer framebuffer)
	{
		vkDestroyFramebuffer(m_device, framebuffer, nullptr);
	}

	void VulkanRHI::updateDescriptorSet(uint32_t write_count, VkWriteDescriptorSet* p_descriptor_write, uint32_t copy_count, const VkCopyDescriptorSet* p_descriptor_copies)
	{
		vkUpdateDescriptorSets(m_device, write_count, p_descriptor_write, copy_count, p_descriptor_copies);
	}

	void VulkanRHI::allocateDescriptorSets(VkDescriptorSetLayout* p_descriptor_set_layout, uint32_t descriptor_set_count, VkDescriptorSet*& p_descriptor_set, VkDescriptorSetAllocateInfo* p_next)
	{
		VkDescriptorSetAllocateInfo allocate_info{};
		allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocate_info.descriptorPool = m_descriptor_pool;
		allocate_info.pNext = p_next;
		allocate_info.pSetLayouts = p_descriptor_set_layout;
		allocate_info.descriptorSetCount = descriptor_set_count;

		auto tmp = new std::vector<VkDescriptorSet>(descriptor_set_count);
		p_descriptor_set = tmp->data();
		if (vkAllocateDescriptorSets(m_device, &allocate_info, p_descriptor_set) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to allcoate descriptor set");
		}
	}

	void VulkanRHI::createStorageBuffer(VkDeviceSize buffer_size, VkBuffer& storage_buffer, VkDeviceMemory& storage_buffer_memory)
	{
		createBuffer(physical_device, m_device, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storage_buffer, storage_buffer_memory);
	}

	void VulkanRHI::cmdBindDescriptorSets(VkPipelineBindPoint bind_point, VkPipelineLayout* pipeline_layout, int first_set, int set_count, const VkDescriptorSet* const* pDescriptorSets, uint32_t dynamic_offset_count, const uint32_t* p_dynamic_offsets)
	{
		std::vector<VkDescriptorSet> vk_descriptor_sets(set_count);
		for (int i = 0; i < set_count; ++i)
		{
			if (pDescriptorSets[i] != nullptr)
			{
				vk_descriptor_sets[i] = *pDescriptorSets[i];
			}
		}
		vkCmdBindDescriptorSets(getCurrentCommandBuffer(), bind_point, *pipeline_layout, first_set, set_count, vk_descriptor_sets.data(), dynamic_offset_count, p_dynamic_offsets);
	}

	void VulkanRHI::createDescriptorSetLayout(VkDescriptorSetLayoutCreateInfo* create_info, const VkAllocationCallbacks* callbacks, VkDescriptorSetLayout*& p_descriptor_set_layout)
	{
		p_descriptor_set_layout = new VkDescriptorSetLayout();
		if (vkCreateDescriptorSetLayout(m_device, create_info, nullptr, p_descriptor_set_layout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor set layout!");
		}
	}

	bool VulkanRHI::createSampler(const SamplerDesc* sampler_desc, Sampler* sampler)
	{
		std::shared_ptr<Sampler_Vulkan > internal_state = std::make_shared<Sampler_Vulkan>();
		sampler->internal_state = internal_state;
		sampler->desc = *sampler_desc;
		VkSamplerCreateInfo create_info{};
		create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		create_info.flags = 0;
		create_info.pNext = nullptr;

		switch (sampler_desc->filter)
		{
		case Filter::MIN_MAG_MIP_POINT:
		case Filter::MINIMUM_MIN_MAG_MIP_POINT:
		case Filter::MAXIMUM_MIN_MAG_MIP_POINT:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_MAG_POINT_MIP_LINEAR:
		case Filter::MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
		case Filter::MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_POINT_MAG_LINEAR_MIP_POINT:
		case Filter::MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
		case Filter::MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_POINT_MAG_MIP_LINEAR:
		case Filter::MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
		case Filter::MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_LINEAR_MAG_MIP_POINT:
		case Filter::MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
		case Filter::MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		case Filter::MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		case Filter::MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_MAG_LINEAR_MIP_POINT:
		case Filter::MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
		case Filter::MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::MIN_MAG_MIP_LINEAR:
		case Filter::MINIMUM_MIN_MAG_MIP_LINEAR:
		case Filter::MAXIMUM_MIN_MAG_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		case Filter::ANISOTROPIC:
		case Filter::MINIMUM_ANISOTROPIC:
		case Filter::MAXIMUM_ANISOTROPIC:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = true;
			create_info.maxAnisotropy = std::min(16.0f, std::max(1.0f, static_cast<float>(sampler_desc->max_anisotropy)));
			create_info.compareEnable = false;
			break;
		case Filter::COMPARISON_MIN_MAG_MIP_POINT:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_MIN_MAG_MIP_LINEAR:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = true;
			break;
		case Filter::COMPARISON_ANISOTROPIC:
			create_info.minFilter = VK_FILTER_LINEAR;
			create_info.magFilter = VK_FILTER_LINEAR;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			create_info.anisotropyEnable = true;
			create_info.maxAnisotropy = std::min(16.0f, std::max(1.0f, static_cast<float>(sampler_desc->max_anisotropy)));
			create_info.compareEnable = true;
			break;
		default:
			create_info.minFilter = VK_FILTER_NEAREST;
			create_info.magFilter = VK_FILTER_NEAREST;
			create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_NEAREST;
			create_info.anisotropyEnable = false;
			create_info.compareEnable = false;
			break;
		}

		VkSamplerReductionModeCreateInfo reductionmode = {};
		reductionmode.sType = VK_STRUCTURE_TYPE_SAMPLER_REDUCTION_MODE_CREATE_INFO;
		if (checkCapability(GraphicsDeviceCapability::SAMPLER_MINMAX))
		{
			switch (sampler_desc->filter)
			{
			case Filter::MINIMUM_MIN_MAG_MIP_POINT:
			case Filter::MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
			case Filter::MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			case Filter::MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
			case Filter::MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
			case Filter::MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			case Filter::MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
			case Filter::MINIMUM_MIN_MAG_MIP_LINEAR:
			case Filter::MINIMUM_ANISOTROPIC:
				reductionmode.reductionMode = VK_SAMPLER_REDUCTION_MODE_MIN;
				create_info.pNext = &reductionmode;
				break;
			case Filter::MAXIMUM_MIN_MAG_MIP_POINT:
			case Filter::MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
			case Filter::MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			case Filter::MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
			case Filter::MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
			case Filter::MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			case Filter::MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
			case Filter::MAXIMUM_MIN_MAG_MIP_LINEAR:
			case Filter::MAXIMUM_ANISOTROPIC:
				reductionmode.reductionMode = VK_SAMPLER_REDUCTION_MODE_MAX;
				create_info.pNext = &reductionmode;
				break;
			default:
				break;
			}
		}

		create_info.addressModeU = convertTextureAddressMode(sampler_desc->address_u);
		create_info.addressModeV = convertTextureAddressMode(sampler_desc->address_v);
		create_info.addressModeW = convertTextureAddressMode(sampler_desc->address_w);
		create_info.compareOp = convertCompareFunction(sampler_desc->comparison_func);
		create_info.minLod = sampler_desc->min_lod;
		create_info.maxLod = sampler_desc->max_lod;
		create_info.mipLodBias = sampler_desc->mip_lod_bias;
		create_info.borderColor = convertSamplerBorderColor(sampler_desc->border_color);
		create_info.unnormalizedCoordinates = VK_FALSE;

		VkResult res = vkCreateSampler(m_device, &create_info, nullptr, &internal_state->resource);
		assert(res == VK_SUCCESS);

		internal_state->index = allocation_handler->bindless_samplers.allocate();
		if (internal_state->index >= 0)
		{
			VkDescriptorImageInfo image_info{};
			image_info.sampler = internal_state->resource;
			VkWriteDescriptorSet write{};
			write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			write.dstBinding = 0;
			write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
			write.descriptorCount = 1;
			write.pImageInfo = &image_info;
			write.dstArrayElement = internal_state->index;
			write.dstSet = allocation_handler->bindless_samplers.descriptor_set;
			vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
		}
		else
		{
			assert(0);
		}

		return true;
	}

	VkBorderColor convertSamplerBorderColor(const SamplerBorderColor& border_color)
	{
		switch (border_color)
		{
		case SamplerBorderColor::TRANSPARENT_BLACK:
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		case SamplerBorderColor::OPAQUE_BLACK:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK;
		case SamplerBorderColor::OPAQUE_WHITE:
			return VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		default:
			return VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
		}
	}

	VkImageLayout convertImageLayout(ResourceState layout)
	{
		switch (layout)
		{
		case qyhs::graphics::ResourceState::UNDEFINED:
			return VK_IMAGE_LAYOUT_UNDEFINED;
		case qyhs::graphics::ResourceState::SHADER_RESOURCE:
			return VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		default:
			return VK_IMAGE_LAYOUT_GENERAL;
		}
	}

	VkCompareOp convertCompareFunction(ComparisonFunc comparion_func)
	{
		switch (comparion_func)
		{
		case ComparisonFunc::NEVER:
			return VK_COMPARE_OP_NEVER;
		case ComparisonFunc::LESS:
			return VK_COMPARE_OP_LESS;
		case ComparisonFunc::EQUAL:
			return VK_COMPARE_OP_EQUAL;
		case ComparisonFunc::LESS_EQUAL:
			return VK_COMPARE_OP_LESS_OR_EQUAL;
		case ComparisonFunc::GREATER:
			return VK_COMPARE_OP_GREATER;
		case ComparisonFunc::NOT_EQUAL:
			return VK_COMPARE_OP_NOT_EQUAL;
		case ComparisonFunc::GREATER_EQUAL:
			return VK_COMPARE_OP_GREATER_OR_EQUAL;
		case ComparisonFunc::ALWAYS:
			return VK_COMPARE_OP_ALWAYS;
		default:
			return VK_COMPARE_OP_NEVER;
		}
	}

	constexpr bool isFormatSRGB(Format format)
	{
		switch (format)
		{
		case Format::R8G8B8A8_UNORM_SRGB:
		case Format::B8G8R8A8_UNORM_SRGB:
		case Format::BC1_UNORM_SRGB:
		case Format::BC2_UNORM_SRGB:
		case Format::BC3_UNORM_SRGB:
		case Format::BC7_UNORM_SRGB:
			return true;
		default:
			return false;
		}
	}

	constexpr bool isFormatDepthSupport(Format format)
	{
		switch (format)
		{
		case Format::D16_UNORM:
		case Format::D32_FLOAT:
		case Format::D32_FLOAT_S8X24_UINT:
		case Format::D24_UNORM_S8_UINT:
			return true;
		default:
			return false;
		}
	}
	constexpr bool isFormatStencilSupport(Format format)
	{
		switch (format)
		{
		case Format::D32_FLOAT_S8X24_UINT:
		case Format::D24_UNORM_S8_UINT:
			return true;
		default:
			return false;
		}
	}

	uint64_t VulkanRHI::getMinOffsetAlignment(const GPUBufferDesc* desc) const
	{
		uint64_t alignment = 1u;
		if (has_flag(desc->bind_flags, BindFlag::CONSTANT_BUFFER))
		{
			alignment = std::max(alignment, m_physical_device_properties_2.properties.limits.minUniformBufferOffsetAlignment);
		}
		if (has_flag(desc->misc_flags, ResourceMiscFlag::BUFFER_RAW) || has_flag(desc->misc_flags, ResourceMiscFlag::BUFFER_STRUCTURED))
		{
			alignment = std::max(alignment, m_physical_device_properties_2.properties.limits.minStorageBufferOffsetAlignment);
		}
		if (desc->format != Format::UNKNOWN || has_flag(desc->misc_flags, ResourceMiscFlag::TYPED_FORMAT_CASTING))
		{
			alignment = std::max(alignment, m_physical_device_properties_2.properties.limits.minTexelBufferOffsetAlignment);
		}
		return alignment;
	}

	bool VulkanRHI::createBuffer(const GPUBufferDesc *desc, GPUBuffer* buffer,const std::function<void(void*)>& init_buffer_callback)
	{
		std::shared_ptr<Buffer_Vulkan> internal_state = std::make_shared<Buffer_Vulkan>();
		internal_state->allocation_handler = allocation_handler;
		buffer->internal_state = internal_state;
		buffer->desc = *desc;
		buffer->mapped_data = nullptr;
		buffer->mapped_size = 0;
		buffer->type = GPUResource::Type::BUFFER;

		VkBufferCreateInfo buffer_info{};
		buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_info.size = buffer->desc.size;
		buffer_info.usage = 0;
		if (has_flag(buffer->desc.bind_flags, BindFlag::VERTEX_BUFFER))
		{
			buffer_info.usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		}
		if (has_flag(buffer->desc.bind_flags, BindFlag::INDEX_BUFFER))
		{
			buffer_info.usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
		}
		if (has_flag(buffer->desc.bind_flags, BindFlag::CONSTANT_BUFFER))
		{
			buffer_info.usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
		}
		if (has_flag(buffer->desc.bind_flags, BindFlag::SHADER_RESOURCE))
		{
			buffer_info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT; // read only ByteAddressBuffer is also storage buffer
			buffer_info.usage |= VK_BUFFER_USAGE_UNIFORM_TEXEL_BUFFER_BIT;
		}
		if (has_flag(buffer->desc.bind_flags, BindFlag::UNORDERED_ACCESS))
		{
			buffer_info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
			buffer_info.usage |= VK_BUFFER_USAGE_STORAGE_TEXEL_BUFFER_BIT;
		}
		if (has_flag(buffer->desc.misc_flags, ResourceMiscFlag::BUFFER_RAW) || has_flag(buffer->desc.misc_flags, ResourceMiscFlag::BUFFER_STRUCTURED))
		{
			buffer_info.usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
		}
		buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;

		buffer_info.flags = 0;
		VmaAllocationCreateInfo alloc_info = {};
		alloc_info.usage = VMA_MEMORY_USAGE_AUTO;
		if (desc->usage == Usage::READBACK)
		{
			buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
			alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
		else if (desc->usage == Usage::UPLOAD)
		{
			buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
			alloc_info.flags = VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT | VMA_ALLOCATION_CREATE_MAPPED_BIT;
		}
		VkResult res;
		res = vmaCreateBuffer(
			allocation_handler->allocator,
			&buffer_info,
			&alloc_info,
			&internal_state->resource,
			&internal_state->allocation,
			nullptr
		);

		if (desc->usage == Usage::READBACK || desc->usage == Usage::UPLOAD)
		{
			buffer->mapped_data = internal_state->allocation->GetMappedData();
			buffer->mapped_size = internal_state->allocation->GetSize();
		}

		//res = vkCreateBuffer(m_device, &buffer_info, nullptr, &internal_state->resource);
		assert(res == VK_SUCCESS);
		if (buffer->desc.usage == Usage::UPLOAD || buffer->desc.usage == Usage::READBACK)
		{
			buffer->mapped_data = internal_state->allocation->GetMappedData();
			buffer->mapped_size = internal_state->allocation->GetSize();
		}

		if (init_buffer_callback != nullptr)
		{
			CopyAllocator::CopyCMD cmd;
			void* mapped_data = nullptr;
			if (desc->usage == Usage::UPLOAD)
			{
				mapped_data = buffer->mapped_data;
			}
			else
			{
				cmd = copy_allocator.allocate(desc->size);
				mapped_data = cmd.upload_buffer.mapped_data;
			}

			init_buffer_callback(mapped_data);

			if(cmd.isValid())
			{
				VkBufferCopy copyRegion = {};
				copyRegion.size = buffer->desc.size;
				copyRegion.srcOffset = 0;
				copyRegion.dstOffset = 0;

				vkCmdCopyBuffer(
					cmd.transfer_command_buffer,
					to_internal(&cmd.upload_buffer)->resource,
					internal_state->resource,
					1,
					&copyRegion
				);

				VkBufferMemoryBarrier2 barrier = {};
				barrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER_2;
				barrier.buffer = internal_state->resource;
				barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
				barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
				barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
				barrier.dstAccessMask = VK_ACCESS_2_MEMORY_READ_BIT | VK_ACCESS_2_MEMORY_WRITE_BIT;
				barrier.size = VK_WHOLE_SIZE;

				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

				if (has_flag(buffer->desc.bind_flags, BindFlag::CONSTANT_BUFFER))
				{
					barrier.dstAccessMask |= VK_ACCESS_2_UNIFORM_READ_BIT;
				}
				if (has_flag(buffer->desc.bind_flags, BindFlag::VERTEX_BUFFER))
				{
					barrier.dstStageMask |= VK_PIPELINE_STAGE_2_VERTEX_ATTRIBUTE_INPUT_BIT;
					barrier.dstAccessMask |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
				}
				if (has_flag(buffer->desc.bind_flags, BindFlag::INDEX_BUFFER))
				{
					barrier.dstStageMask |= VK_PIPELINE_STAGE_2_INDEX_INPUT_BIT;
					barrier.dstAccessMask |= VK_ACCESS_2_INDEX_READ_BIT;
				}
				if (has_flag(buffer->desc.bind_flags, BindFlag::SHADER_RESOURCE))
				{
					barrier.dstAccessMask |= VK_ACCESS_2_SHADER_READ_BIT;
				}
				if (has_flag(buffer->desc.bind_flags, BindFlag::UNORDERED_ACCESS))
				{
					barrier.dstAccessMask |= VK_ACCESS_2_SHADER_READ_BIT;
					barrier.dstAccessMask |= VK_ACCESS_2_SHADER_WRITE_BIT;
				}
				

				VkDependencyInfo dependencyInfo = {};
				dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				dependencyInfo.bufferMemoryBarrierCount = 1;
				dependencyInfo.pBufferMemoryBarriers = &barrier;

				vkCmdPipelineBarrier2(cmd.transition_command_buffer, &dependencyInfo);

				copy_allocator.submit(cmd);
			}
		}

		if (!has_flag(desc->misc_flags, ResourceMiscFlag::NO_DEFAULT_DESCRIPTORS))
		{
			if (has_flag(desc->bind_flags, BindFlag::SHADER_RESOURCE))
			{
				createSubresource(buffer, SubresourceType::SRV, 0);
			}
			if (has_flag(desc->bind_flags, BindFlag::UNORDERED_ACCESS))
			{
				createSubresource(buffer, SubresourceType::UAV, 0);
			}
		}
		return res == VK_SUCCESS;
	}

	constexpr Format getFormatNonSRGB(Format format)
	{
		switch (format)
		{
		case Format::R8G8B8A8_UNORM_SRGB:
			return Format::R8G8B8A8_UNORM;
		case Format::B8G8R8A8_UNORM_SRGB:
			return Format::B8G8R8A8_UNORM;
		case Format::BC1_UNORM_SRGB:
			return Format::BC1_UNORM;
		case Format::BC2_UNORM_SRGB:
			return Format::BC2_UNORM;
		case Format::BC3_UNORM_SRGB:
			return Format::BC3_UNORM;
		case Format::BC7_UNORM_SRGB:
			return Format::BC7_UNORM;
		default:
			return format;
		}
	}

	int VulkanRHI::createSubresource(Texture* texture, SubresourceType type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount, const Format* format_change, const ImageAspect* aspect, const Swizzle* swizzle, float min_lod_clamp) const
	{
		auto internal_state = to_internal(texture);
		Texture_Vulkan::TextureSubresource subresource;
		subresource.base_mip_level = firstMip;
		subresource.mip_count = mipCount;
		subresource.base_array_layer = firstSlice;
		subresource.layer_count = sliceCount;

		VkImageViewCreateInfo view_info = {};
		view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		view_info.image = internal_state->resource;
		if (texture->desc.type == TextureDesc::Type::TEXTURE_1D)
		{
			if (texture->desc.array_size > 1)
			{
				view_info.viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY;
			}
			else
			{
				view_info.viewType = VK_IMAGE_VIEW_TYPE_1D;
			}
		}
		else if (texture->desc.type == TextureDesc::Type::TEXTURE_2D)
		{
			if (texture->desc.array_size > 1)
			{
				if (has_flag(texture->desc.misc_flags, ResourceMiscFlag::TEXTURE_CUBE))
				{
					if (texture->desc.array_size > 6 && sliceCount > 6)
					{
						view_info.viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY;
					}
					else
					{
						view_info.viewType = VK_IMAGE_VIEW_TYPE_CUBE;
					}
				}
				else
				{
					if (texture->desc.array_size > 1)
					{
						view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
					}
					else
					{
						view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
					}
				}
				view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			}
			else
			{
				view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
			}
		}
		else if (texture->desc.type == TextureDesc::Type::TEXTURE_3D)
		{
			view_info.viewType = VK_IMAGE_VIEW_TYPE_3D;
		}

		view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		if (aspect != nullptr)
		{
			view_info.subresourceRange.aspectMask = _convertImageAspectMask(*aspect);
		}
		view_info.subresourceRange.baseArrayLayer = firstSlice;
		view_info.subresourceRange.layerCount = sliceCount;
		view_info.subresourceRange.baseMipLevel = firstMip;
		view_info.subresourceRange.levelCount = mipCount;

		if (type == SubresourceType::SRV)
		{
			view_info.components = _convertSwizzle(swizzle == nullptr ? texture->desc.swizzle : *swizzle);
		}
		Format format = texture->desc.format;
		view_info.format = convertFormat(format);

		switch (type)
		{
		case qyhs::graphics::SubresourceType::SRV:
		{
			if (isFormatDepthSupport(format))
			{
				view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			}
			VkImageViewUsageCreateInfo viewUsageInfo = {};
			viewUsageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_USAGE_CREATE_INFO;
			viewUsageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT;
			view_info.pNext = &viewUsageInfo;

			VkResult res = vkCreateImageView(m_device, &view_info, nullptr, &subresource.image_view);
			assert(res == VK_SUCCESS);

			subresource.index = allocation_handler->bindless_sampled_images.allocate();
			if (subresource.index >= 0)
			{
				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageView = subresource.image_view;
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
				VkWriteDescriptorSet write = {};
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
				write.dstBinding = 0;
				write.dstArrayElement = subresource.index;
				write.descriptorCount = 1;
				write.dstSet = allocation_handler->bindless_sampled_images.descriptor_set;
				write.pImageInfo = &imageInfo;
				vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
			}
			if (!internal_state->srv.isValid())
			{
				internal_state->srv = subresource;
				return -1;
			}
			try
			{
				std::cout << "size = " << internal_state->subresources_srv.size()
					<< ", capacity = " << internal_state->subresources_srv.capacity() << std::endl;
				internal_state->subresources_srv.push_back(subresource);
			}
			catch (const std::bad_array_new_length& e) {
				std::cout << "Caught exception: " << e.what() << std::endl;
			}

			return int(internal_state->subresources_srv.size() - 1);
		}
		break;
		case qyhs::graphics::SubresourceType::UAV:
		{
			if (view_info.viewType == VK_IMAGE_VIEW_TYPE_CUBE || view_info.viewType == VK_IMAGE_VIEW_TYPE_CUBE_ARRAY)
			{
				view_info.viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY;
			}
			VkResult res = vkCreateImageView(m_device, &view_info, nullptr, &subresource.image_view);
			assert(res == VK_SUCCESS);

			subresource.index = allocation_handler->bindless_storage_images.allocate();
			if (subresource.index >= 0)
			{
				VkDescriptorImageInfo imageInfo = {};
				imageInfo.imageView = subresource.image_view;
				imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;
				VkWriteDescriptorSet write = {};
				write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
				write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
				write.dstBinding = 0;
				write.dstArrayElement = subresource.index;
				write.descriptorCount = 1;
				write.dstSet = allocation_handler->bindless_storage_images.descriptor_set;
				write.pImageInfo = &imageInfo;
				vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
			}

			if (!internal_state->uav.isValid())
			{
				internal_state->uav = subresource;
				return -1;
			}
			internal_state->subresources_uav.push_back(subresource);
			return int(internal_state->subresources_uav.size() - 1);
		}
		break;
		case qyhs::graphics::SubresourceType::RTV:
		{
			view_info.subresourceRange.levelCount = 1;
			VkResult res = vkCreateImageView(m_device, &view_info, nullptr, &subresource.image_view);

			if (res == VK_SUCCESS)
			{
				if (!internal_state->rtv.isValid())
				{
					internal_state->rtv = subresource;
					internal_state->framebuffer_layercount = view_info.subresourceRange.layerCount;
					return -1;
				}
				internal_state->subresources_rtv.push_back(subresource);
				return int(internal_state->subresources_rtv.size() - 1);
			}
			else
			{
				assert(0);
			}
		}
		break;
		case qyhs::graphics::SubresourceType::DSV:
		{
			view_info.subresourceRange.levelCount = 1;
			view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

			VkResult res = vkCreateImageView(m_device, &view_info, nullptr, &subresource.image_view);

			if (res == VK_SUCCESS)
			{
				if (!internal_state->dsv.isValid())
				{
					internal_state->dsv = subresource;
					internal_state->framebuffer_layercount = view_info.subresourceRange.layerCount;
					return -1;
				}
				internal_state->subresources_dsv.push_back(subresource);
				return int(internal_state->subresources_dsv.size() - 1);
			}
			else
			{
				assert(0);
			}
		}
		break;
		default:
			break;
		}
	}

	int VulkanRHI::createSubresource(GPUBuffer* buffer, SubresourceType type, uint64_t offset, uint64_t size, const Format* format_change)
	{
		Buffer_Vulkan* internal_state = to_internal(buffer);
		Buffer_Vulkan::BufferSubresource subresource;
		Format format = buffer->desc.format;
		const GPUBufferDesc& desc = buffer->desc;
		VkResult res;
		if (format_change != nullptr)
		{
			format = *format_change;
		}
		if (type == SubresourceType::UAV)
		{
			format = getFormatNonSRGB(format);
		}
		subresource.buffer_info.buffer = internal_state->resource;
		subresource.buffer_info.range = size;
		subresource.buffer_info.offset = offset;
		switch (type)
		{
		case qyhs::graphics::SubresourceType::SRV:
		case qyhs::graphics::SubresourceType::UAV:
		{
			if (format == Format::UNKNOWN)
			{
				// Raw buffer
				subresource.index = allocation_handler->bindless_storage_buffers.allocate();
				subresource.is_typed = false;
				if (subresource.isValid())
				{
					subresource.buffer_info.buffer = internal_state->resource;
					subresource.buffer_info.offset = offset;
					subresource.buffer_info.range = size;

					VkWriteDescriptorSet write = {};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
					write.dstBinding = 0;
					write.dstArrayElement = subresource.index;
					write.descriptorCount = 1;
					write.dstSet = allocation_handler->bindless_storage_buffers.descriptor_set;
					write.pBufferInfo = &subresource.buffer_info;
					vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
				}

				if (type == SubresourceType::SRV)
				{
					if (!internal_state->srv.isValid())
					{
						internal_state->srv = subresource;
						return -1;
					}
					internal_state->subresources_srv.push_back(subresource);
					return int(internal_state->subresources_srv.size() - 1);
				}
				else
				{
					if (!internal_state->uav.isValid())
					{
						internal_state->uav = subresource;
						return -1;
					}
					internal_state->subresources_uav.push_back(subresource);
					return int(internal_state->subresources_uav.size() - 1);
				}
			}
			else
			{
				// Typed buffer
				VkBufferViewCreateInfo srv_desc = {};
				srv_desc.sType = VK_STRUCTURE_TYPE_BUFFER_VIEW_CREATE_INFO;
				srv_desc.buffer = internal_state->resource;
				srv_desc.flags = 0;
				srv_desc.format = convertFormat(format);
				srv_desc.offset = offset;
				srv_desc.range = std::min(size, (uint64_t)desc.size - srv_desc.offset);

				res = vkCreateBufferView(m_device, &srv_desc, nullptr, &subresource.buffer_view);

				if (res == VK_SUCCESS)
				{
					subresource.is_typed = true;

					if (type == SubresourceType::SRV)
					{
						subresource.index = allocation_handler->bindless_uniform_texel_buffers.allocate();
						if (subresource.isValid())
						{
							VkWriteDescriptorSet write = {};
							write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
							write.dstBinding = 0;
							write.dstArrayElement = subresource.index;
							write.descriptorCount = 1;
							write.dstSet = allocation_handler->bindless_uniform_texel_buffers.descriptor_set;
							write.pTexelBufferView = &subresource.buffer_view;
							vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
						}

						if (!internal_state->srv.isValid())
						{
							internal_state->srv = subresource;
							return -1;
						}
						internal_state->subresources_srv.push_back(subresource);
						return int(internal_state->subresources_srv.size() - 1);
					}
					else
					{
						subresource.index = allocation_handler->bindless_storage_texel_buffers.allocate();
						if (subresource.isValid())
						{
							VkWriteDescriptorSet write = {};
							write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
							write.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
							write.dstBinding = 0;
							write.dstArrayElement = subresource.index;
							write.descriptorCount = 1;
							write.dstSet = allocation_handler->bindless_storage_texel_buffers.descriptor_set;
							write.pTexelBufferView = &subresource.buffer_view;
							vkUpdateDescriptorSets(m_device, 1, &write, 0, nullptr);
						}

						if (!internal_state->uav.isValid())
						{
							internal_state->uav = subresource;
							return -1;
						}
						internal_state->subresources_uav.push_back(subresource);
						return int(internal_state->subresources_uav.size() - 1);
					}
				}
				else
				{
					assert(0);
				}
			}
		}
		break;
		default:
			assert(0);
			break;
		}
		return -1;
	}

	void VulkanRHI::bindScissorRects(uint32_t scissor_count, const Rect* rects, CommandList cmd)
	{
		assert(rects != nullptr);
		VkRect2D scissors[16];
		assert(scissor_count <= arraysize(scissors));
		assert(scissor_count <= m_physical_device_properties_2.properties.limits.maxViewports);
		for (uint32_t i = 0; i < scissor_count; ++i)
		{
			scissors[i].extent.width = abs(rects[i].right - rects[i].left);
			scissors[i].extent.height = abs(rects[i].top - rects[i].bottom);
			scissors[i].offset.x = std::max(0, rects[i].left);
			scissors[i].offset.y = std::max(0, rects[i].top);
		}
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		vkCmdSetScissorWithCount(commandlist.getCommandBuffer(), scissor_count, scissors);
	}

	void VulkanRHI::bindPipelineState(const PipelineState* pso, CommandList cmd)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		auto internal_state = to_internal(pso);
		if (internal_state->pipeline != VK_NULL_HANDLE)
		{
			if (commandlist.active_pso != pso)
			{
				vkCmdBindPipeline(commandlist.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, internal_state->pipeline);
			}
			commandlist.prev_pipeline_hash = 0;
			commandlist.dirty_pso = false;
		}
		else
		{
			size_t pipeline_hash = 0;
			helper::hash_combine(pipeline_hash, internal_state->hash);
			helper::hash_combine(pipeline_hash, commandlist.renderpass_info.get_hash());

			//TODO:
			if (commandlist.prev_pipeline_hash == pipeline_hash)
			{
				commandlist.active_pso = pso;
				//vkCmdBindPipeline(commandlist.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, internal_state->pipeline);
				return;
			}

			commandlist.prev_pipeline_hash = pipeline_hash;
			commandlist.dirty_pso = true;
		}

		if (commandlist.active_pso == nullptr)
		{
			commandlist.binder.dirty |= DescriptorBinder::DIRTY_ALL;
		}
		else
		{
			auto active_internal = to_internal(commandlist.active_pso);
			if (internal_state->binding_hash != active_internal->binding_hash)
			{
				commandlist.binder.dirty |= DescriptorBinder::DIRTY_ALL;
			}
		}
		if (!internal_state->bindlessSets.empty())
		{
			vkCmdBindDescriptorSets(commandlist.getCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, internal_state->pipeline_layout, internal_state->bindlessFirstSet,
				(uint32_t)internal_state->bindlessSets.size(), internal_state->bindlessSets.data(), 0, nullptr);

		}

		commandlist.active_pso = pso;
	}

	void VulkanRHI::setName(Shader* shader, const char* name) const
	{
		if (!enable_debug_utils_label || shader == nullptr || !shader->isValid())
			return;

		VkDebugUtilsObjectNameInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		info.pObjectName = name;
		info.objectType = VK_OBJECT_TYPE_SHADER_MODULE;
		info.objectHandle = (uint64_t)to_internal(shader)->shader_module;

		if (info.objectHandle == (uint64_t)VK_NULL_HANDLE)
			return;

		VkResult res = vkSetDebugUtilsObjectNameEXT(m_device, &info);
		assert(res == VK_SUCCESS);
	}

	constexpr VkAccessFlags2 _ParseResourceState(ResourceState value)
	{
		VkAccessFlags2 flags = 0;

		if (has_flag(value, ResourceState::SHADER_RESOURCE))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
		}
		if (has_flag(value, ResourceState::SHADER_RESOURCE_COMPUTE))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
		}
		if (has_flag(value, ResourceState::UNORDERED_ACCESS))
		{
			flags |= VK_ACCESS_2_SHADER_READ_BIT;
			flags |= VK_ACCESS_2_SHADER_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::COPY_SRC))
		{
			flags |= VK_ACCESS_2_TRANSFER_READ_BIT;
		}
		if (has_flag(value, ResourceState::COPY_DST))
		{
			flags |= VK_ACCESS_2_TRANSFER_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::RENDERTARGET))
		{
			flags |= VK_ACCESS_2_COLOR_ATTACHMENT_READ_BIT;
			flags |= VK_ACCESS_2_COLOR_ATTACHMENT_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::DEPTHSTENCIL))
		{
			flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
			flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		}
		if (has_flag(value, ResourceState::DEPTHSTENCIL_READONLY))
		{
			flags |= VK_ACCESS_2_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		}
		if (has_flag(value, ResourceState::VERTEX_BUFFER))
		{
			flags |= VK_ACCESS_2_VERTEX_ATTRIBUTE_READ_BIT;
		}
		if (has_flag(value, ResourceState::INDEX_BUFFER))
		{
			flags |= VK_ACCESS_2_INDEX_READ_BIT;
		}
		if (has_flag(value, ResourceState::CONSTANT_BUFFER))
		{
			flags |= VK_ACCESS_2_UNIFORM_READ_BIT;
		}
		if (has_flag(value, ResourceState::INDIRECT_ARGUMENT))
		{
			flags |= VK_ACCESS_2_INDIRECT_COMMAND_READ_BIT;
		}
		if (has_flag(value, ResourceState::PREDICATION))
		{
			flags |= VK_ACCESS_2_CONDITIONAL_RENDERING_READ_BIT_EXT;
		}
		if (has_flag(value, ResourceState::SHADING_RATE_SOURCE))
		{
			flags |= VK_ACCESS_2_FRAGMENT_SHADING_RATE_ATTACHMENT_READ_BIT_KHR;
		}
		if (has_flag(value, ResourceState::VIDEO_DECODE_DST))
		{
			flags |= VK_ACCESS_2_VIDEO_DECODE_WRITE_BIT_KHR;
		}
		if (has_flag(value, ResourceState::VIDEO_DECODE_SRC))
		{
			flags |= VK_ACCESS_2_VIDEO_DECODE_READ_BIT_KHR;
		}

		return flags;
	}

	void VulkanRHI::setName(GPUResource* resource, const char* name) const
	{
		if (!enable_debug_utils_label || resource == nullptr || !resource->isValid())
			return;

		VkDebugUtilsObjectNameInfoEXT info{ VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT };
		info.pObjectName = name;
		if (resource->isTexture())
		{
			info.objectType = VK_OBJECT_TYPE_IMAGE;
			info.objectHandle = (uint64_t)to_internal((const Texture*)resource)->resource;
		}
		else if (resource->isBuffer())
		{
			info.objectType = VK_OBJECT_TYPE_BUFFER;
			info.objectHandle = (uint64_t)to_internal((const GPUBuffer*)resource)->resource;
		}

		if (info.objectHandle == (uint64_t)VK_NULL_HANDLE)
			return;

		VkResult res = vkSetDebugUtilsObjectNameEXT(m_device, &info);
		assert(res == VK_SUCCESS);
	}

	void VulkanRHI::drawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd)
	{
		predraw(cmd);
		CommandList_Vulkan& commandlist = getCommandList(cmd);
		vkCmdDraw(commandlist.getCommandBuffer(), vertexCount, instanceCount, startVertexLocation, startInstanceLocation);
	}

	bool VulkanRHI::createTexture(const TextureDesc* desc, graphics::Texture* texture, SubresourceData* initial_data)
	{
		std::shared_ptr<Texture_Vulkan >internal_state = std::make_shared<Texture_Vulkan>();

		texture->internal_state = internal_state;
		texture->desc = *desc;
		texture->type = GPUResource::Type::TEXTURE;

		if (texture->desc.mip_levels == 0)
		{
			texture->desc.mip_levels = getMipCount(texture->desc.width, texture->desc.height, texture->desc.depth);
		}
		VkImageCreateInfo image_info{};
		image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		image_info.extent.width = texture->desc.width;
		image_info.extent.height = texture->desc.height;
		image_info.extent.depth = texture->desc.depth;
		image_info.format = convertFormat(texture->desc.format);
		image_info.arrayLayers = texture->desc.array_size;
		image_info.mipLevels = texture->desc.mip_levels;
		image_info.samples = (VkSampleCountFlagBits)texture->desc.sample_count;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.tiling = VK_IMAGE_TILING_OPTIMAL;
		image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		image_info.usage = 0;

		if (has_flag(texture->desc.bind_flags, BindFlag::SHADER_RESOURCE))
		{
			image_info.usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
		}
		if (has_flag(texture->desc.bind_flags, BindFlag::UNORDERED_ACCESS))
		{
			image_info.usage |= VK_IMAGE_USAGE_STORAGE_BIT;

			if (isFormatSRGB(texture->desc.format))
			{
				image_info.flags |= VK_IMAGE_CREATE_EXTENDED_USAGE_BIT;		//allow other different usage of imageview to access the image
			}
		}
		if (has_flag(texture->desc.bind_flags, BindFlag::RENDER_TARGET))
		{
			image_info.usage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		}
		if (has_flag(texture->desc.bind_flags, BindFlag::DEPTH_STENCIL))
		{
			image_info.usage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
		}
		//VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR specifies that 
		// the image can be used to create a VkImageView suitable for 
		// use as a fragment shading rate attachment or shading rate image
		if (has_flag(texture->desc.bind_flags, BindFlag::SHADING_RATE))
		{
			image_info.usage |= VK_IMAGE_USAGE_FRAGMENT_SHADING_RATE_ATTACHMENT_BIT_KHR;
		}
		if (has_flag(texture->desc.misc_flags, ResourceMiscFlag::TRANSIENT_ATTACHMENT))
		{
			image_info.usage |= VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT;
		}
		else
		{
			image_info.usage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
			image_info.usage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
		}

		image_info.flags = 0;
		if (has_flag(texture->desc.misc_flags, ResourceMiscFlag::TEXTURE_CUBE))
		{
			image_info.flags |= VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT;
		}

		if (families.size() > 0)
		{
			image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
			image_info.queueFamilyIndexCount = (uint32_t)families.size();
			image_info.pQueueFamilyIndices = families.data();
		}
		else
		{
			image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}

		switch (texture->desc.type)
		{
		case TextureDesc::Type::TEXTURE_1D:
			image_info.imageType = VK_IMAGE_TYPE_1D;
			break;
		case TextureDesc::Type::TEXTURE_2D:
			image_info.imageType = VK_IMAGE_TYPE_2D;
			break;
		case TextureDesc::Type::TEXTURE_3D:
			image_info.imageType = VK_IMAGE_TYPE_3D;
			break;
		default:
			assert(0);
			break;
		}
		VkResult res;
		{
			VmaAllocationCreateInfo allocInfo = {};
			allocInfo.usage = VMA_MEMORY_USAGE_AUTO;
			VmaAllocator allocator = allocation_handler->allocator;
			VkExternalMemoryImageCreateInfo externalMemImageCreateInfo = {};
			res = vmaCreateImage(
				allocator,
				&image_info,
				&allocInfo,
				&internal_state->resource,
				&internal_state->allocation,
				nullptr
			);
			assert(res == VK_SUCCESS);
			if (initial_data != nullptr)
			{
				CopyAllocator::CopyCMD cmd;
				void* mapped_data = nullptr;
				if (desc->usage == Usage::UPLOAD)
				{
					mapped_data = texture->mapped_data;
				}
				else
				{
					cmd = copy_allocator.allocate(internal_state->allocation->GetSize());
					mapped_data = cmd.upload_buffer.mapped_data;
				}

				std::vector<VkBufferImageCopy> copyRegions;

				VkDeviceSize copyOffset = 0;
				uint32_t initDataIdx = 0;
				for (uint32_t layer = 0; layer < desc->array_size; ++layer)
				{
					uint32_t width = image_info.extent.width;
					uint32_t height = image_info.extent.height;
					uint32_t depth = image_info.extent.depth;
					for (uint32_t mip = 0; mip < desc->mip_levels; ++mip)
					{
						const SubresourceData& subresourceData = initial_data[initDataIdx++];
						const uint32_t block_size = graphics::getFormatBlockSize(desc->format);
						const uint32_t num_blocks_x = std::max(1u, width / block_size);
						const uint32_t num_blocks_y = std::max(1u, height / block_size);
						const uint32_t dst_rowpitch = num_blocks_x * GetFormatStride(desc->format);
						const uint32_t dst_slicepitch = dst_rowpitch * num_blocks_y;
						const uint32_t src_rowpitch = subresourceData.row_pitch;
						const uint32_t src_slicepitch = subresourceData.slice_pitch;
						for (uint32_t z = 0; z < depth; ++z)
						{
							uint8_t* dst_slice = (uint8_t*)mapped_data + copyOffset + dst_slicepitch * z;
							uint8_t* src_slice = (uint8_t*)subresourceData.data_ptr + src_slicepitch * z;
							for (uint32_t y = 0; y < num_blocks_y; ++y)
							{
								std::memcpy(
									dst_slice + dst_rowpitch * y,
									src_slice + src_rowpitch * y,
									dst_rowpitch
								);
							}
						}

						if (cmd.isValid())
						{
							VkBufferImageCopy copyRegion = {};
							copyRegion.bufferOffset = copyOffset;
							copyRegion.bufferRowLength = 0;
							copyRegion.bufferImageHeight = 0;

							copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
							copyRegion.imageSubresource.mipLevel = mip;
							copyRegion.imageSubresource.baseArrayLayer = layer;
							copyRegion.imageSubresource.layerCount = 1;

							copyRegion.imageOffset = { 0, 0, 0 };
							copyRegion.imageExtent = {
								width,
								height,
								depth
							};

							copyRegions.push_back(copyRegion);
						}

						copyOffset += dst_slicepitch * depth;
						copyOffset = alignTo(copyOffset, VkDeviceSize(4)); // fix for validation: on transfer queue the srcOffset must be 4-byte aligned

						width = std::max(1u, width / 2);
						height = std::max(1u, height / 2);
						depth = std::max(1u, depth / 2);
					}
				}

				if (cmd.isValid())
				{
					VkImageMemoryBarrier2 barrier = {};
					barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
					barrier.image = internal_state->resource;
					barrier.oldLayout = image_info.initialLayout;
					barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.srcStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
					barrier.srcAccessMask = 0;
					barrier.dstStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
					barrier.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
					barrier.subresourceRange.baseArrayLayer = 0;
					barrier.subresourceRange.layerCount = VK_REMAINING_ARRAY_LAYERS;
					barrier.subresourceRange.baseMipLevel = 0;
					barrier.subresourceRange.levelCount = VK_REMAINING_MIP_LEVELS;
					barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
					barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

					VkDependencyInfo dependencyInfo = {};
					dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
					dependencyInfo.imageMemoryBarrierCount = 1;
					dependencyInfo.pImageMemoryBarriers = &barrier;

					vkCmdPipelineBarrier2(cmd.transfer_command_buffer, &dependencyInfo);

					vkCmdCopyBufferToImage(
						cmd.transfer_command_buffer,
						to_internal(&cmd.upload_buffer)->resource,
						internal_state->resource,
						VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
						(uint32_t)copyRegions.size(),
						copyRegions.data()
					);

					std::swap(barrier.srcStageMask, barrier.dstStageMask);
					barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
					barrier.newLayout = convertImageLayout(texture->desc.layout);
					barrier.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
					barrier.dstAccessMask = _ParseResourceState(texture->desc.layout);
					vkCmdPipelineBarrier2(cmd.transfer_command_buffer, &dependencyInfo);

					copy_allocator.submit(cmd);
				}
			}
			else if (texture->desc.layout != ResourceState::UNDEFINED && internal_state->resource != VK_NULL_HANDLE)
			{
				VkImageMemoryBarrier2 barrier = {};
				barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
				barrier.image = internal_state->resource;
				barrier.oldLayout = image_info.initialLayout;
				barrier.newLayout = convertImageLayout(texture->desc.layout);
				barrier.srcStageMask = VK_PIPELINE_STAGE_2_TRANSFER_BIT;
				barrier.srcAccessMask = 0;
				barrier.dstStageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
				barrier.dstAccessMask = convertResourceState(texture->desc.layout);
				if (isFormatDepthSupport(texture->desc.format))
				{
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
					if (isFormatStencilSupport(texture->desc.format))
					{
						barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
					}
				}
				else
				{
					barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
				}
				barrier.subresourceRange.baseArrayLayer = 0;
				barrier.subresourceRange.layerCount = image_info.arrayLayers;
				barrier.subresourceRange.baseMipLevel = 0;
				barrier.subresourceRange.levelCount = image_info.mipLevels;
				barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
				barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

				CopyAllocator::CopyCMD cmd = copy_allocator.allocate(0);

				VkDependencyInfo dependencyInfo = {};
				dependencyInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
				dependencyInfo.imageMemoryBarrierCount = 1;
				dependencyInfo.pImageMemoryBarriers = &barrier;

				vkCmdPipelineBarrier2(cmd.transition_command_buffer, &dependencyInfo);
				copy_allocator.submit(cmd);
			}

			if (!has_flag(desc->misc_flags, ResourceMiscFlag::NO_DEFAULT_DESCRIPTORS))
			{
				if (has_flag(texture->desc.bind_flags, BindFlag::RENDER_TARGET))
				{
					createSubresource(texture, SubresourceType::RTV, 0, -1, 0, -1);
				}
				if (has_flag(texture->desc.bind_flags, BindFlag::DEPTH_STENCIL))
				{
					createSubresource(texture, SubresourceType::DSV, 0, -1, 0, -1);
				}
				if (has_flag(texture->desc.bind_flags, BindFlag::SHADER_RESOURCE))
				{
					createSubresource(texture, SubresourceType::SRV, 0, -1, 0, -1);
				}
				if (has_flag(texture->desc.bind_flags, BindFlag::UNORDERED_ACCESS))
				{
					createSubresource(texture, SubresourceType::UAV, 0, -1, 0, -1);
				}
			}
			return res == VK_SUCCESS;
		}

		if (!has_flag(desc->misc_flags, ResourceMiscFlag::NO_DEFAULT_DESCRIPTORS))
		{
			if (has_flag(desc->bind_flags, BindFlag::RENDER_TARGET))
			{
				createSubresource(texture, SubresourceType::RTV, 0, -1, 0, -1);
			}
			if (has_flag(desc->bind_flags, BindFlag::SHADER_RESOURCE))
			{
				createSubresource(texture, SubresourceType::SRV, 0, -1, 0, -1);
			}
			if (has_flag(desc->bind_flags, BindFlag::DEPTH_STENCIL))
			{
				createSubresource(texture, SubresourceType::DSV, 0, -1, 0, -1);
			}
			if (has_flag(desc->bind_flags, BindFlag::UNORDERED_ACCESS))
			{
				createSubresource(texture, SubresourceType::UAV, 0, -1, 0, -1);
			}
		}

		return res == VK_SUCCESS;
	}

	RenderPassInfo VulkanRHI::getRenderPassInfo(CommandList cmd) const
	{
		return getCommandList(cmd).renderpass_info;
	}

	VkSamplerAddressMode VulkanRHI::convertTextureAddressMode(const TextureAddressMode& address_mode)
	{
		switch (address_mode)
		{
		case TextureAddressMode::WRAP:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		case TextureAddressMode::MIRROR:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case TextureAddressMode::CLAMP:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case TextureAddressMode::BORDER:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
		case TextureAddressMode::MIRROR_ONCE:
			if (m_physical_device_features_1_2.samplerMirrorClampToEdge == VK_TRUE)
			{
				return VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE;
			}
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		default:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		}
	}

	void VulkanRHI::updateUniformBuffer() {

	}

	void VulkanRHI::resetFence()
	{
		vkResetFences(m_device, 1, &m_is_frame_in_flight_fences[m_current_frame_index]);
	}

	void VulkanRHI::resetCommandBuffer()
	{
		vkResetCommandBuffer(m_command_buffers[m_current_frame_index], /*VkCommandBufferResetFlagBits*/ 0);
	}

	void VulkanRHI::resetCommandPool()
	{
		if (vkResetCommandPool(m_device, m_command_pools[m_current_frame_index], 0) != VK_SUCCESS)
		{
			throw std::runtime_error("failed to reset command pool");
		}
	}

	//TODO:now the descriptor is not correct
	void VulkanRHI::createDescriptorPool() {
		std::array<VkDescriptorPoolSize, 5> poolSizes{};
		poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 100);

		poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolSizes[1].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 100);

		poolSizes[2].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 100);

		poolSizes[3].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		poolSizes[3].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 100);

		poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		poolSizes[4].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 100);

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
		poolInfo.pPoolSizes = poolSizes.data();
		poolInfo.maxSets = static_cast<uint32_t>(100);


		if (vkCreateDescriptorPool(m_device, &poolInfo, nullptr, &m_descriptor_pool) != VK_SUCCESS) {
			throw std::runtime_error("failed to create descriptor pool!");
		}
	}

	void VulkanRHI::createCommandBuffers() {
		m_command_buffers.resize(MAX_FRAMES_IN_FLIGHT);

		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = command_pool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1U;
		for (uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i)
		{
			allocInfo.commandPool = m_command_pools[i];
			if (vkAllocateCommandBuffers(m_device, &allocInfo, &m_command_buffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to allocate command buffers!");
			}
		}
	}

	void VulkanRHI::createSyncObjects() {
		imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
		m_is_frame_in_flight_fences.resize(MAX_FRAMES_IN_FLIGHT);

		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &imageAvailableSemaphores[i]) != VK_SUCCESS ||
				vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &renderFinishedSemaphores[i]) != VK_SUCCESS ||
				vkCreateFence(m_device, &fenceInfo, nullptr, &m_is_frame_in_flight_fences[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create synchronization objects for a frame!");
			}
		}
	}

	void VulkanRHI::createAllocator()
	{
		VmaVulkanFunctions vulkanFunctions = {};
		vulkanFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		vulkanFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo create_info = {};
		create_info.vulkanApiVersion = m_vulkan_api_version;
		create_info.physicalDevice = physical_device;
		create_info.device = m_device;
		create_info.instance = instance;
		create_info.pVulkanFunctions = &vulkanFunctions;

		vmaCreateAllocator(&create_info, &m_assets_allocator);
	}

	void VulkanRHI::prepareContext()
	{
		m_p_current_frame_index = &m_current_frame_index;
		m_current_command_buffer = m_command_buffers[m_current_frame_index];
		m_p_command_buffers = m_command_buffers.data();
		m_p_command_pools = m_command_pools;
	}

	void VulkanRHI::generateMipmaps(VkImage image, VkFormat format, uint32_t tex_width, uint32_t tex_height)
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(physical_device, format, &properties);
		if (!(properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
		{
			throw std::runtime_error("texture image doesn't support linear blitting!");
		}
		VkCommandBuffer command_buffer = beginSingleTimeCommands();
		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.image = image;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;
		barrier.subresourceRange.levelCount = 1;

		int mip_width = tex_width;
		int mip_height = tex_height;
		for (uint32_t i = 1; i < mip_levels; ++i)
		{
			barrier.subresourceRange.baseMipLevel = i - 1;
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);

			VkImageBlit blit{};
			blit.srcOffsets[0] = { 0,0,0 };
			blit.srcOffsets[1] = { mip_width,mip_height,1 };
			blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.srcSubresource.baseArrayLayer = 0;
			blit.srcSubresource.layerCount = 1;
			blit.srcSubresource.mipLevel = i - 1;

			blit.dstOffsets[0] = { 0,0,0 };
			blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1,mip_height > 1 ? mip_height / 2 : 1,1 };
			blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
			blit.dstSubresource.baseArrayLayer = 0;
			blit.dstSubresource.mipLevel = i;
			blit.dstSubresource.layerCount = 1;


			vkCmdBlitImage(command_buffer, image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &blit, VK_FILTER_LINEAR);
			barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
			barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			vkCmdPipelineBarrier(command_buffer,
				VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
				0, nullptr,
				0, nullptr,
				1, &barrier);
			if (mip_width > 1) mip_width /= 2;
			if (mip_height > 1)mip_height /= 2;
		}
		barrier.subresourceRange.baseMipLevel = mip_levels - 1;
		barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
		barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		vkCmdPipelineBarrier(command_buffer, VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
			0, nullptr,
			0, nullptr,
			1, &barrier);

		endSingleTimeCommands(command_buffer);
	}

	VkCommandBuffer VulkanRHI::beginSingleTimeCommands() {
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandPool = command_pool;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void VulkanRHI::endSingleTimeCommands(VkCommandBuffer commandBuffer) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &commandBuffer;
		std::lock_guard<std::mutex> lock(queue_submit_mutex);
		vkQueueSubmit(graphics_queue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphics_queue);

		vkFreeCommandBuffers(m_device, command_pool, 1, &commandBuffer);
	}

	void VulkanRHI::waitForFence()
	{
		vkWaitForFences(m_device, 1, &m_is_frame_in_flight_fences[m_current_frame_index], VK_TRUE, UINT64_MAX);
	}

	void VulkanRHI::getNextImage(std::function<void()> update_pass_after_recreate_swap_chain)
	{
		VkResult result = vkAcquireNextImageKHR(m_device, swap_chain, UINT64_MAX, imageAvailableSemaphores[m_current_frame_index], VK_NULL_HANDLE, &current_image_index);

		if (result == VK_ERROR_OUT_OF_DATE_KHR) {
			recreateSwapChain();
			update_pass_after_recreate_swap_chain();
			return;
		}
		else if (result == VK_SUBOPTIMAL_KHR)
		{
			recreateSwapChain();
			update_pass_after_recreate_swap_chain();
			system("pause");
		}
		else if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
			throw std::runtime_error("failed to acquire swap chain image!");
		}
	}

	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface) {
		SwapChainSupportDetails details;
		VkResult result;
		result = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &details.capabilities);
		assert(result == VK_SUCCESS);
		uint32_t formatCount;
		result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
		assert(result == VK_SUCCESS);
		if (formatCount != 0) {
			details.formats.resize(formatCount);
			result = vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, details.formats.data());
			assert(result == VK_SUCCESS);
		}

		uint32_t presentModeCount;
		result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
		assert(result == VK_SUCCESS);
		if (presentModeCount != 0) {
			details.present_modes.resize(presentModeCount);
			result = vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, details.present_modes.data());
			assert(result == VK_SUCCESS);
		}

		return details;
	}

	bool VulkanRHI::isDeviceSuitable(VkPhysicalDevice device) {

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);
		if (!supportedFeatures.samplerAnisotropy)
		{
			return false;
		}
		bool extensionsSupported = checkDeviceExtensionSupport(device);
		return  extensionsSupported;
	}

	bool VulkanRHI::checkExtensionSupport(const char* check_extension, const std::vector<VkExtensionProperties>& available_extensions)
	{
		for (const VkExtensionProperties& property : available_extensions)
		{
			if (strcmp(property.extensionName, check_extension) == 0)
			{
				return true;
			}
		}
		return false;
	}

	bool VulkanRHI::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const std::string& check_extension : requiredExtensions)
		{
			if (!checkExtensionSupport(check_extension.c_str(), availableExtensions))
			{
				return false;
			}
		}
		void** features_chain = &m_physical_device_features_1_3.pNext;
		enabled_extensions = deviceExtensions;
		if (checkExtensionSupport(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME, availableExtensions))
		{
			enabled_extensions.push_back(VK_EXT_DEPTH_CLIP_ENABLE_EXTENSION_NAME);
			depth_clip_enable_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DEPTH_CLIP_ENABLE_FEATURES_EXT;
			*features_chain = &depth_clip_enable_features;
			features_chain = &depth_clip_enable_features.pNext;
		}

		return true;
	}

	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {
		for (const auto& availableFormat : availableFormats) {
			if (availableFormat.format == VK_FORMAT_B8G8R8A8_SRGB && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR) {
				return availableFormat;
			}
		}

		return availableFormats[0];
	}

	VkPresentModeKHR VulkanRHI::chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes) {
		for (const auto& availablePresentMode : availablePresentModes) {
			if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
				return availablePresentMode;
			}
		}

		return VK_PRESENT_MODE_FIFO_KHR;
	}

	VkExtent2D VulkanRHI::chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities) {
		if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
			return capabilities.currentExtent;
		}
		else {
			int width, height;
			glfwGetFramebufferSize(m_window, &width, &height);

			VkExtent2D actualExtent = {
				static_cast<uint32_t>(width),
				static_cast<uint32_t>(height)
			};

			actualExtent.width = std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
			actualExtent.height = std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);
			return actualExtent;
		}
	}

	VulkanRHI::CommandList_Vulkan& VulkanRHI::getCommandList(CommandList cmd)const
	{
		auto cmd_list = static_cast<CommandList_Vulkan*>(cmd.internal_state);
		return *cmd_list;
	}

	VkImageView VulkanRHI::createImageView(VkImage image, VkFormat format, uint32_t mip_levels, VkImageAspectFlags aspectFlags) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = mip_levels;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		VkImageView imageView;
		if (vkCreateImageView(m_device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}

		return imageView;
	}

	void VulkanRHI::createImage(uint32_t width, uint32_t height, VkFormat format, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.extent.width = width;
		imageInfo.extent.height = height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mip_levels;
		imageInfo.arrayLayers = 1;
		imageInfo.format = format;
		imageInfo.tiling = tiling;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		imageInfo.usage = usage;
		imageInfo.samples = num_samples;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateImage(m_device, &imageInfo, nullptr, &image) != VK_SUCCESS) {
			throw std::runtime_error("failed to create image!");
		}

		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(m_device, image, &memRequirements);

		VkMemoryAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(m_device, &allocInfo, nullptr, &imageMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}

		vkBindImageMemory(m_device, image, imageMemory, 0);
	}

	VkResult VulkanRHI::CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger)
	{
		auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

	void VulkanRHI::DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, debugMessenger, pAllocator);
		}
	}

	void VulkanRHI::initInstanceFunction()
	{
		_vkCmdBeginDebugUtilsLabelEXT = (PFN_vkCmdBeginDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdBeginDebugUtilsLabelEXT");
		_vkCmdEndDebugUtilsLabelEXT = (PFN_vkCmdEndDebugUtilsLabelEXT)vkGetInstanceProcAddr(instance, "vkCmdEndDebugUtilsLabelEXT");
	}

	void VulkanRHI::transitionImageLayout(VkImage image, VkFormat format, uint32_t mip_levels, VkImageLayout oldLayout, VkImageLayout newLayout) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkImageMemoryBarrier barrier{};
		barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		barrier.oldLayout = oldLayout;
		barrier.newLayout = newLayout;
		barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
		barrier.image = image;
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		barrier.subresourceRange.baseMipLevel = 0;
		barrier.subresourceRange.levelCount = mip_levels;
		barrier.subresourceRange.baseArrayLayer = 0;
		barrier.subresourceRange.layerCount = 1;

		VkPipelineStageFlags sourceStage;
		VkPipelineStageFlags destinationStage;

		if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
			barrier.srcAccessMask = 0;
			barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

			sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
			destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		}
		else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
			barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
			barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

			sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
			destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		}
		else {
			throw std::invalid_argument("unsupported layout transition!");
		}

		vkCmdPipelineBarrier(
			commandBuffer,
			sourceStage, destinationStage,
			0,
			0, nullptr,
			0, nullptr,
			1, &barrier
		);

		endSingleTimeCommands(commandBuffer);
	}

	void VulkanRHI::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands();

		VkBufferImageCopy region{};
		region.bufferOffset = 0;
		region.bufferRowLength = 0;
		region.bufferImageHeight = 0;
		region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
		region.imageSubresource.mipLevel = 0;
		region.imageSubresource.baseArrayLayer = 0;
		region.imageSubresource.layerCount = 1;
		region.imageOffset = { 0, 0, 0 };
		region.imageExtent = {
			width,
			height,
			1
		};

		vkCmdCopyBufferToImage(commandBuffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

		endSingleTimeCommands(commandBuffer);
	}

	uint32_t VulkanRHI::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	VkFormat VulkanRHI::findDepthFormat() {
		return findSupportedFormat(
			{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
			VK_IMAGE_TILING_OPTIMAL,
			VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	VkFormat VulkanRHI::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physical_device, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			}
			else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;
			}
		}

		throw std::runtime_error("failed to find supported format!");
	}

	VkShaderModule VulkanRHI::createShaderModule(const std::vector<char>& code) {
		VkShaderModuleCreateInfo createInfo{};
		createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
		createInfo.codeSize = code.size();
		createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

		VkShaderModule shaderModule;
		if (vkCreateShaderModule(m_device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
			throw std::runtime_error("failed to create shader module!");
		}

		return shaderModule;
	}
	void VulkanRHI::CommandQueue::submit(VulkanRHI* rhi, VkFence fence)
	{
		if (queue == VK_NULL_HANDLE)
		{
			return;
		}
		std::scoped_lock lock(*locker);
		VkSubmitInfo2 submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		submit_info.commandBufferInfoCount = (uint32_t)submit_cmds.size();
		submit_info.pCommandBufferInfos = submit_cmds.data();

		submit_info.waitSemaphoreInfoCount = (uint32_t)submit_waitSemaphoreInfos.size();
		submit_info.pWaitSemaphoreInfos = submit_waitSemaphoreInfos.data();

		submit_info.signalSemaphoreInfoCount = (uint32_t)submit_signalSemaphoreInfos.size();
		submit_info.pSignalSemaphoreInfos = submit_signalSemaphoreInfos.data();

		VkResult res = vkQueueSubmit2(queue, 1, &submit_info, fence);
		assert(res == VK_SUCCESS);

		if (!submit_swapchains.empty())
		{
			VkPresentInfoKHR present_info{};
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.swapchainCount = (uint32_t)submit_swapchains.size();
			present_info.pSwapchains = submit_swapchains.data();
			present_info.pImageIndices = submit_swapchain_image_indices.data();
			present_info.waitSemaphoreCount = (uint32_t)submit_signalSemaphores.size();
			present_info.pWaitSemaphores = submit_signalSemaphores.data();
			res = vkQueuePresentKHR(queue, &present_info);
			if (res != VK_SUCCESS)
			{
				// Handle outdated error in present:
				if (res == VK_SUBOPTIMAL_KHR || res == VK_ERROR_OUT_OF_DATE_KHR)
				{
					for (auto& swapchain : swapchain_updates)
					{
						auto internal_state = to_internal(&swapchain);
						bool success = createInternalSwapChain(rhi, internal_state.get());
						assert(success);
					}
				}
				else
				{
					assert(0);
				}
			}
			swapchain_updates.clear();
			submit_swapchains.clear();
			submit_swapchain_image_indices.clear();
			submit_waitSemaphoreInfos.clear();
			submit_signalSemaphores.clear();
			submit_signalSemaphoreInfos.clear();
			submit_cmds.clear();

		}
	}

	void VulkanRHI::CommandQueue::signal(VkSemaphore& semaphore)
	{
		VkSemaphoreSubmitInfo& signal_submit_info = submit_signalSemaphoreInfos.emplace_back();
		signal_submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signal_submit_info.semaphore = semaphore;
		signal_submit_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		signal_submit_info.value = 0;

	}

	void VulkanRHI::CommandQueue::wait(VkSemaphore& semaphore)
	{
		VkSemaphoreSubmitInfo& wait_submit_info = submit_waitSemaphoreInfos.emplace_back();
		wait_submit_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
		wait_submit_info.semaphore = semaphore;
		wait_submit_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
		wait_submit_info.value = 0;
	}

	void VulkanRHI::CopyAllocator::submit(CopyCMD& cmd)
	{
		VkResult res = vkEndCommandBuffer(cmd.transfer_command_buffer);
		assert(res == VK_SUCCESS);
		res = vkEndCommandBuffer(cmd.transition_command_buffer);
		assert(res == VK_SUCCESS);
		VkSubmitInfo2 submit_info{};
		submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO_2;
		VkCommandBufferSubmitInfo command_buffer_submit_info{};
		command_buffer_submit_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_SUBMIT_INFO;

		VkSemaphoreSubmitInfo  signal_semaphore_infos[2] = {};
		signal_semaphore_infos[0].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		signal_semaphore_infos[1].sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;

		VkSemaphoreSubmitInfo  wait_semaphore_info{};
		wait_semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SUBMIT_INFO;
		{
			signal_semaphore_infos[0].semaphore = cmd.semaphores[0];
			signal_semaphore_infos[0].stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;
			command_buffer_submit_info.commandBuffer = cmd.transfer_command_buffer;
			submit_info.commandBufferInfoCount = 1;
			submit_info.pCommandBufferInfos = &command_buffer_submit_info;
			submit_info.signalSemaphoreInfoCount = 1;
			submit_info.pSignalSemaphoreInfos = signal_semaphore_infos;
			std::scoped_lock lock(*rhi->queues[QUEUE_TRANSFER].locker);
			res = vkQueueSubmit2(rhi->queues[QUEUE_TRANSFER].queue, 1, &submit_info, VK_NULL_HANDLE);
			assert(res == VK_SUCCESS);
		}

		{
			wait_semaphore_info.semaphore = cmd.semaphores[0];
			wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

			command_buffer_submit_info.commandBuffer = cmd.transition_command_buffer;
			signal_semaphore_infos[0].semaphore = cmd.semaphores[1];
			signal_semaphore_infos[0].stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

			submit_info.waitSemaphoreInfoCount = 1;
			submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
			submit_info.commandBufferInfoCount = 1;
			submit_info.pCommandBufferInfos = &command_buffer_submit_info;
			if (rhi->queues[QUEUE_VIDEO_DECODE].queue != VK_NULL_HANDLE)
			{
				signal_semaphore_infos[1].semaphore = cmd.semaphores[2]; // signal for video decode queue
				signal_semaphore_infos[1].stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT; // signal for video decode queue
				submit_info.signalSemaphoreInfoCount = 2;
			}
			else
			{
				submit_info.signalSemaphoreInfoCount = 1;
			}
			submit_info.pSignalSemaphoreInfos = signal_semaphore_infos;
			std::scoped_lock lock(*rhi->queues[QUEUE_GRAPHICS].locker);
			res = vkQueueSubmit2(rhi->queues[QUEUE_GRAPHICS].queue, 1, &submit_info, VK_NULL_HANDLE);
			assert(res == VK_SUCCESS);
		}

		if (rhi->queues[QUEUE_VIDEO_DECODE].queue != VK_NULL_HANDLE)
		{
			wait_semaphore_info.semaphore = cmd.semaphores[2]; // wait for graphics queue
			wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

			submit_info.waitSemaphoreInfoCount = 1;
			submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
			submit_info.commandBufferInfoCount = 0;
			submit_info.pCommandBufferInfos = nullptr;
			submit_info.signalSemaphoreInfoCount = 0;
			submit_info.pSignalSemaphoreInfos = nullptr;

			std::scoped_lock lock(*rhi->queues[QUEUE_VIDEO_DECODE].locker);
			res = vkQueueSubmit2(rhi->queues[QUEUE_VIDEO_DECODE].queue, 1, &submit_info, VK_NULL_HANDLE);
			assert(res == VK_SUCCESS);
		}

		// This must be final submit in this function because it will also signal a fence for pso tracking by CPU!
		{
			wait_semaphore_info.semaphore = cmd.semaphores[1]; // wait for graphics queue
			wait_semaphore_info.stageMask = VK_PIPELINE_STAGE_2_ALL_COMMANDS_BIT;

			submit_info.waitSemaphoreInfoCount = 1;
			submit_info.pWaitSemaphoreInfos = &wait_semaphore_info;
			submit_info.commandBufferInfoCount = 0;
			submit_info.pCommandBufferInfos = nullptr;
			submit_info.signalSemaphoreInfoCount = 0;
			submit_info.pSignalSemaphoreInfos = nullptr;

			std::scoped_lock lock(*rhi->queues[QUEUE_COMPUTE].locker);
			res = vkQueueSubmit2(rhi->queues[QUEUE_COMPUTE].queue, 1, &submit_info, cmd.fence); // final submit also signals fence!
			assert(res == VK_SUCCESS);
		}

		std::scoped_lock lock(locker);
		free_cmd_list.push_back(cmd);


	}

	void VulkanRHI::DescriptorBinder::flush(bool graphics, CommandList cmd)
	{
		if (dirty & DIRTY_NONE)
		{
			return;
		}
		CommandList_Vulkan& commandlist = rhi->getCommandList(cmd);

		VkPipelineLayout pipeline_layout = VK_NULL_HANDLE;
		VkDescriptorSet descriptor_set = VK_NULL_HANDLE;
		VkDescriptorSetLayout descriptor_set_layout = VK_NULL_HANDLE;
		PipelineState_Vulkan* pso = to_internal(commandlist.active_pso);
		uint32_t uniform_buffer_dynamic_count = 0;
		if (graphics)
		{
			pipeline_layout = pso->pipeline_layout;
			descriptor_set = descriptorset_graphics;
			descriptor_set_layout = pso->descriptorSetLayout;
			uniform_buffer_dynamic_count = pso->uniform_buffer_dynamic_slots.size();
			for (size_t i = 0; i < pso->uniform_buffer_dynamic_slots.size(); ++i)
			{
				uniform_buffer_dynamic_offsets[i] = (uint32_t)table.CBV_offset[pso->uniform_buffer_dynamic_slots[i]];
			}
		}
		else
		{

		}

		VkPipelineBindPoint pipeline_bind_point = VK_PIPELINE_BIND_POINT_GRAPHICS;
		if (!graphics)
		{
			pipeline_bind_point = VK_PIPELINE_BIND_POINT_COMPUTE;
			if (commandlist.active_cs->stage == ShaderStage::LIBRARY)
			{
				pipeline_bind_point = VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR;
			}
		}
		const auto& layout_bindings = pso->layout_bindings;
		const auto& image_view_types = pso->imageViewTypes;
		int i = 0;
		if (dirty & DIRTY_DESCRIPTOR)
		{
			auto& binder_pool = commandlist.binder_pools[rhi->getBufferIndex()];

			VkDescriptorSetAllocateInfo allocate_info = {};
			allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocate_info.descriptorPool = binder_pool.descriptor_pool;
			allocate_info.pSetLayouts = &descriptor_set_layout;
			allocate_info.descriptorSetCount = 1;

			VkResult res = vkAllocateDescriptorSets(rhi->m_device, &allocate_info, &descriptor_set);
			if (res == VK_ERROR_OUT_OF_POOL_MEMORY)
			{
				binder_pool.pool_size *= 2;
				binder_pool.destroy();
				binder_pool.init(rhi);
				allocate_info.descriptorPool = binder_pool.descriptor_pool;
				res = vkAllocateDescriptorSets(rhi->m_device, &allocate_info, &descriptor_set);
			}
			assert(res == VK_SUCCESS);

			descriptor_set_writes.clear();
			buffer_infos.clear();
			image_infos.clear();

			size_t total_buffer_infos = 0;
			size_t total_image_infos = 0;
			for (auto& x : layout_bindings) {
				if (x.pImmutableSamplers != nullptr) {
					continue;
				}
				for (int descriptor_index = 0; descriptor_index < x.descriptor_count; ++descriptor_index) {
					switch (x.descriptorType) {
					case VK_DESCRIPTOR_TYPE_SAMPLER:
					case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
						total_image_infos++;
						break;
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
						total_buffer_infos++;
						break;
						// 处理其他需要的类型...
					default:
						break;
					}
				}
			}
			buffer_infos.reserve(total_buffer_infos);
			image_infos.reserve(total_image_infos);
			descriptor_set_writes.reserve(total_buffer_infos + total_image_infos);

			for (auto& x : layout_bindings)
			{
				if (x.pImmutableSamplers != nullptr)
				{
					++i;
					continue;
				}
				VkImageViewType image_view_type = image_view_types[i++];
				for (int descriptor_index = 0; descriptor_index < x.descriptor_count; ++descriptor_index)
				{
					uint32_t unroll_binding = x.binding + descriptor_index;
					auto& write = descriptor_set_writes.emplace_back();
					write = {};
					write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
					write.dstSet = descriptor_set;
					write.dstArrayElement = descriptor_index;
					write.dstBinding = x.binding;
					write.descriptorCount = 1;
					write.descriptorType = x.descriptorType;
					switch (x.descriptorType)
					{
					case VK_DESCRIPTOR_TYPE_SAMPLER:
					{
						write.pImageInfo = &image_infos.emplace_back();
						image_infos.back() = {};

						const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_S;
						const Sampler& sampler = table.SAMPLER[original_binding];
						if (!sampler.isValid())
						{
							image_infos.back().sampler = rhi->null_sampler;
						}
						else
						{
							Sampler_Vulkan* sam = to_internal(&sampler);
							image_infos.back().sampler = to_internal(&sampler)->resource;
						}
					}
					break;

					case VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE:
					{
						image_infos.emplace_back();
						write.pImageInfo = &image_infos.back();
						image_infos.back() = {};

						const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_T;
						const GPUResource& resource = table.SHADER_RESOURCE_VIEW[original_binding];
						if (!resource.isValid() || !resource.isTexture())
						{
							switch (image_view_type)
							{
							case VK_IMAGE_VIEW_TYPE_1D:
								image_infos.back().imageView = rhi->nullImageView1D;
								break;
							case VK_IMAGE_VIEW_TYPE_2D:
								image_infos.back().imageView = rhi->nullImageView2D;
								break;
							case VK_IMAGE_VIEW_TYPE_3D:
								image_infos.back().imageView = rhi->nullImageView3D;
								break;
							case VK_IMAGE_VIEW_TYPE_CUBE:
								image_infos.back().imageView = rhi->nullImageViewCube;
								break;
							case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
								image_infos.back().imageView = rhi->nullImageView1DArray;
								break;
							case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
								image_infos.back().imageView = rhi->nullImageView2DArray;
								break;
							case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
								image_infos.back().imageView = rhi->nullImageViewCubeArray;
								break;
							case VK_IMAGE_VIEW_TYPE_MAX_ENUM:
								break;
							default:
								break;
							}
							image_infos.back().imageLayout = VK_IMAGE_LAYOUT_GENERAL;
						}
						else
						{
							int subresource = table.SRV_index[original_binding];
							auto texture_internal = to_internal((const Texture*)&resource);
							auto& subresource_descriptor = subresource >= 0 ? texture_internal->subresources_srv[subresource] : texture_internal->srv;
							image_infos.back().imageView = subresource_descriptor.image_view;
							image_infos.back().imageLayout = texture_internal->default_layout;
						}
					}
					break;

					case VK_DESCRIPTOR_TYPE_STORAGE_IMAGE:
					{
						image_infos.emplace_back();
						write.pImageInfo = &image_infos.back();
						image_infos.back() = {};
						image_infos.back().imageLayout = VK_IMAGE_LAYOUT_GENERAL;

						const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_U;
						const GPUResource& resource = table.UNORDER_ACCESS_VIEW[original_binding];
						if (!resource.isValid() || !resource.isTexture())
						{
							switch (image_view_type)
							{
							case VK_IMAGE_VIEW_TYPE_1D:
								image_infos.back().imageView = rhi->nullImageView1D;
								break;
							case VK_IMAGE_VIEW_TYPE_2D:
								image_infos.back().imageView = rhi->nullImageView2D;
								break;
							case VK_IMAGE_VIEW_TYPE_3D:
								image_infos.back().imageView = rhi->nullImageView3D;
								break;
							case VK_IMAGE_VIEW_TYPE_CUBE:
								image_infos.back().imageView = rhi->nullImageViewCube;
								break;
							case VK_IMAGE_VIEW_TYPE_1D_ARRAY:
								image_infos.back().imageView = rhi->nullImageView1DArray;
								break;
							case VK_IMAGE_VIEW_TYPE_2D_ARRAY:
								image_infos.back().imageView = rhi->nullImageView2DArray;
								break;
							case VK_IMAGE_VIEW_TYPE_CUBE_ARRAY:
								image_infos.back().imageView = rhi->nullImageViewCubeArray;
								break;
							case VK_IMAGE_VIEW_TYPE_MAX_ENUM:
								break;
							default:
								break;
							}
						}
						else
						{
							int subresource = table.UAV_index[original_binding];
							auto texture_internal = to_internal((const Texture*)&resource);
							auto& subresource_descriptor = subresource >= 0 ? texture_internal->subresources_uav[subresource] : texture_internal->uav;
							image_infos.back().imageView = subresource_descriptor.image_view;
						}
					}
					break;

					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER:
					{
						buffer_infos.emplace_back();
						buffer_infos.back() = {};
						write.pBufferInfo = &buffer_infos.back();

						const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_B;
						const GPUBuffer& buffer = table.CONSTANT_BUFFER_VIEW[original_binding];
						uint64_t offset = table.CBV_offset[original_binding];

						if (!buffer.isValid())
						{
							buffer_infos.back().buffer = rhi->nullBuffer;
							buffer_infos.back().range = VK_WHOLE_SIZE;
						}
						else
						{
							auto internal_state = to_internal(&buffer);
							buffer_infos.back().buffer = internal_state->resource;
							buffer_infos.back().offset = offset;
							if (graphics)
							{
								buffer_infos.back().range = pso->uniform_buffer_sizes[original_binding];
							}
							else
							{
								//buffer_infos.back().range = cs_internal->uniform_buffer_sizes[original_binding];
							}
							if (buffer_infos.back().range == 0ull)
							{
								buffer_infos.back().range = VK_WHOLE_SIZE;
							}
						}
					}
					break;

					case VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC:
					{
						VkDescriptorBufferInfo& buffer_info = buffer_infos.emplace_back();
						buffer_info = {};
						write.pBufferInfo = &buffer_info;
						

						const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_B;
						const GPUBuffer& buffer = table.CONSTANT_BUFFER_VIEW[original_binding];

						if (!buffer.isValid())
						{
							buffer_infos.back().buffer = rhi->nullBuffer;
							buffer_infos.back().range = VK_WHOLE_SIZE;
						}
						else
						{
							auto internal_state = to_internal(&buffer);
							buffer_infos.back().buffer = internal_state->resource;
							if (graphics)
							{
								buffer_infos.back().range = pso->uniform_buffer_sizes[original_binding];
							}
							else
							{
								//buffer_infos.back().range = cs_internal->uniform_buffer_sizes[original_binding];
							}
							if (buffer_infos.back().range == 0ull)
							{
								buffer_infos.back().range = VK_WHOLE_SIZE;
							}
						}
						
					}
					break;

					/*case VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER:
					{
						texelBufferViews.emplace_back();
						write.pTexelBufferView = &texelBufferViews.back();
						texelBufferViews.back() = {};

						const uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_T;
						const GPUResource& resource = table.SRV[original_binding];
						if (!resource.IsValid() || !resource.IsBuffer())
						{
							texelBufferViews.back() = device->nullBufferView;
						}
						else
						{
							int subresource = table.SRV_index[original_binding];
							auto buffer_internal = to_internal((const GPUBuffer*)&resource);
							auto& subresource_descriptor = subresource >= 0 ? buffer_internal->subresources_srv[subresource] : buffer_internal->srv;
							texelBufferViews.back() = subresource_descriptor.buffer_view;
						}
					}
					break;*/

					/*case VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER:
					{
						texelBufferViews.emplace_back();
						write.pTexelBufferView = &texelBufferViews.back();
						texelBufferViews.back() = {};

						const uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_U;
						const GPUResource& resource = table.UAV[original_binding];
						if (!resource.IsValid() || !resource.IsBuffer())
						{
							texelBufferViews.back() = device->nullBufferView;
						}
						else
						{
							int subresource = table.UAV_index[original_binding];
							auto buffer_internal = to_internal((const GPUBuffer*)&resource);
							auto& subresource_descriptor = subresource >= 0 ? buffer_internal->subresources_uav[subresource] : buffer_internal->uav;
							texelBufferViews.back() = subresource_descriptor.buffer_view;
						}
					}
					break;*/

					case VK_DESCRIPTOR_TYPE_STORAGE_BUFFER:
					{
						buffer_infos.emplace_back();
						buffer_infos.back() = {};
						write.pBufferInfo = &buffer_infos.back();

						if (x.binding < VULKAN_BINDING_SHIFT_U)
						{
							// SRV
							const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_T;
							const GPUResource& resource = table.SHADER_RESOURCE_VIEW[original_binding];
							if (!resource.isValid() || !resource.isBuffer())
							{
								buffer_infos.back().buffer = rhi->nullBuffer;
								buffer_infos.back().range = VK_WHOLE_SIZE;
							}
							else
							{
								int subresource = table.SRV_index[original_binding];
								auto buffer_internal = to_internal((const GPUBuffer*)&resource);
								auto& subresource_descriptor = subresource >= 0 ? buffer_internal->subresources_srv[subresource] : buffer_internal->srv;
								buffer_infos.back() = subresource_descriptor.buffer_info;
							}
						}
						else
						{
							// UNORDER_ACCESS_VIEW
							const uint32_t original_binding = unroll_binding - VULKAN_BINDING_SHIFT_U;
							const GPUResource& resource = table.UNORDER_ACCESS_VIEW[original_binding];
							if (!resource.isValid() || !resource.isBuffer())
							{
								buffer_infos.back().buffer = rhi->nullBuffer;
								buffer_infos.back().range = VK_WHOLE_SIZE;
							}
							else
							{
								int subresource = table.UAV_index[original_binding];
								auto buffer_internal = to_internal((const GPUBuffer*)&resource);
								auto& subresource_descriptor = subresource >= 0 ? buffer_internal->subresources_uav[subresource] : buffer_internal->uav;
								buffer_infos.back() = subresource_descriptor.buffer_info;
							}
						}
					}
					break;

					//case VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR:
					//{
					//	accelerationStructureViews.emplace_back();
					//	write.pNext = &accelerationStructureViews.back();
					//	accelerationStructureViews.back() = {};
					//	accelerationStructureViews.back().sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
					//	accelerationStructureViews.back().accelerationStructureCount = 1;

					//	const uint32_t original_binding = unrolled_binding - VULKAN_BINDING_SHIFT_T;
					//	const GPUResource& resource = table.SRV[original_binding];
					//	if (!resource.IsValid() || !resource.IsAccelerationStructure())
					//	{
					//		assert(0); // invalid acceleration structure!
					//	}
					//	else
					//	{
					//		auto as_internal = to_internal((const RaytracingAccelerationStructure*)&resource);
					//		accelerationStructureViews.back().pAccelerationStructures = &as_internal->resource;
					//	}
					//}
					//break;
					default:
						break;
					}
				}

			}
			vkUpdateDescriptorSets(rhi->getDevice(), (uint32_t)descriptor_set_writes.size(),
				descriptor_set_writes.data(), 0, nullptr);
		}

		if (dirty & DIRTY_OFFSET)
		{

		}

		vkCmdBindDescriptorSets(commandlist.getCommandBuffer(), pipeline_bind_point, pipeline_layout, 0, 1, &descriptor_set,
			uniform_buffer_dynamic_count, uniform_buffer_dynamic_offsets);

		if (graphics)
		{
			descriptorset_graphics = descriptor_set;
		}
		else
		{
			//descriptorset_compute = descriptor_set;
		}
		dirty = DIRTY_NONE;
	}

	void VulkanRHI::DescriptorBinderPool::init(VulkanRHI* vulkan_rhi)
	{
		rhi = vulkan_rhi;

		VkResult res;

		VkDescriptorPoolSize pool_sizes[10] = {};
		uint32_t count = 0;

		pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		pool_sizes[0].descriptorCount = DESCRIPTORBINDER_CBV_COUNT * pool_size;
		count++;

		pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
		pool_sizes[1].descriptorCount = DESCRIPTORBINDER_CBV_COUNT * pool_size;
		count++;

		pool_sizes[2].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		pool_sizes[2].descriptorCount = DESCRIPTORBINDER_SRV_COUNT * pool_size;
		count++;

		pool_sizes[3].type = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;
		pool_sizes[3].descriptorCount = DESCRIPTORBINDER_SRV_COUNT * pool_size;
		count++;

		pool_sizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		pool_sizes[4].descriptorCount = DESCRIPTORBINDER_SRV_COUNT * pool_size;
		count++;

		pool_sizes[5].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
		pool_sizes[5].descriptorCount = DESCRIPTORBINDER_UAV_COUNT * pool_size;
		count++;

		pool_sizes[6].type = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;
		pool_sizes[6].descriptorCount = DESCRIPTORBINDER_UAV_COUNT * pool_size;
		count++;

		pool_sizes[7].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		pool_sizes[7].descriptorCount = DESCRIPTORBINDER_UAV_COUNT * pool_size;
		count++;

		pool_sizes[8].type = VK_DESCRIPTOR_TYPE_SAMPLER;
		pool_sizes[8].descriptorCount = DESCRIPTORBINDER_SAMPLER_COUNT * pool_size;
		count++;

		if (rhi->checkCapability(GraphicsDeviceCapability::RAYTRACING))
		{
			pool_sizes[9].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
			pool_sizes[9].descriptorCount = DESCRIPTORBINDER_SRV_COUNT * pool_size;
			count++;
		}

		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = count;
		poolInfo.pPoolSizes = pool_sizes;
		poolInfo.maxSets = pool_size;

		res = vkCreateDescriptorPool(rhi->m_device, &poolInfo, nullptr, &descriptor_pool);
		assert(res == VK_SUCCESS);
	}

}