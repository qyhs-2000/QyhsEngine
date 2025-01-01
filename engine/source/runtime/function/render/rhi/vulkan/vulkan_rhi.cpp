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


#ifdef USE_VOLK
#define VOLK_IMPLEMENTATION
#include "vulkan/volk.h"

#endif // USE_VOLK

#include <tiny_obj_loader.h>
#include <core/base/macro.h>



namespace QYHS
{
	SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device, VkSurfaceKHR& surface);
	VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
	void Texture2D::fromBuffer(unsigned char* buffer, unsigned int buffer_size, VkFormat format, unsigned int width, unsigned int height, VkDevice device)
	{
		assert(buffer);
		uint32_t mip_levels = std::floor(std::log2(width > height ? width : height));

		VkBufferCreateInfo buffer_create_info{};
		buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buffer_create_info.size = static_cast<VkDeviceSize>(buffer_size);
		buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

		VkBuffer stage_buffer;

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


		initVolk();
		createInstance();
		pickPhysicalDevice();
		createLogicalDevice();
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

	bool checkValidationLayerSupport() {
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
		//cmd_vulkan.reset(getBufferIndex());
		cmd_vulkan.queue = queue;
		cmd_vulkan.index = current_cmd;
		if (cmd_vulkan.getCommandBuffer() == VK_NULL_HANDLE)
		{
			for (int i = 0; i < BUFFER_COUNT; ++i)
			{
				VkCommandPoolCreateInfo pool_nfo = {};
				pool_nfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
				switch (queue)
				{
				case QUEUE_GRAPHICS:
					pool_nfo.queueFamilyIndex = queue_family_indices.graphics_family.value();
					break;
				case QUEUE_COMPUTE:
					pool_nfo.queueFamilyIndex = queue_family_indices.compute_family.value();
					break;
				default:
					assert(0); // queue type not handled
					break;
				}
				pool_nfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;

				result = vkCreateCommandPool(m_device, &pool_nfo, nullptr, &cmd_vulkan.command_pools[i][queue]);
				assert(result == VK_SUCCESS);

				VkCommandBufferAllocateInfo allocate_info = {};
				allocate_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
				allocate_info.commandBufferCount = 1;
				allocate_info.commandPool = cmd_vulkan.command_pools[i][queue];
				allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
				assert(vkAllocateCommandBuffers(m_device, &allocate_info, &cmd_vulkan.command_buffers[i][queue]) == VK_SUCCESS);
			}
		}
		result = vkResetCommandPool(m_device, cmd_vulkan.getCommandPool(), 0);
		assert(result == VK_SUCCESS);

		VkCommandBufferBeginInfo begin_info{};
		begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		begin_info.pInheritanceInfo = nullptr; // Optional

		result = vkBeginCommandBuffer(cmd_vulkan.getCommandBuffer(), &begin_info);
		assert(result == VK_SUCCESS);
		return cmd;
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
		case QYHS::FORMAT_R8G8B8_UNORM:
			texture_byte_size = width * height * 3;
			vulkan_image_format = VK_FORMAT_R8G8_UNORM;
			break;
		case QYHS::FORMAT_R8G8B8_SRGB:
			texture_byte_size = width * height * 3;
			vulkan_image_format = VK_FORMAT_R8G8_SRGB;
			break;
		case QYHS::FORMAT_R8G8B8A8_UNORM:
			texture_byte_size = width * height * 4;
			vulkan_image_format = VK_FORMAT_R8G8B8A8_UNORM;
			break;
		case QYHS::FORMAT_R8G8B8A8_SRGB:
			texture_byte_size = width * height * 4;
			vulkan_image_format = VK_FORMAT_R8G8B8A8_SRGB;
			break;
		case QYHS::FORMAT_R32G32_FLOAT:
			texture_byte_size = width * height * 4 * 2;
			vulkan_image_format = VK_FORMAT_R32G32_SFLOAT;
			break;
		case QYHS::FORMAT_R32G32B32_FLOAT:
			texture_byte_size = width * height * 4 * 3;
			vulkan_image_format = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case QYHS::FORMAT_R32G32B32A32_FLOAT:
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
		std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;

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



	constexpr VkFormat VulkanRHI::convertFormat(Format value)
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

	void VulkanRHI::createLogicalDevice() {
		queue_family_indices = findQueueFamilies(physical_device);

		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		std::set<uint32_t> uniqueQueueFamilies = { queue_family_indices.graphics_family.value(),queue_family_indices.compute_family.value() };

		float queuePriority = 1.0f;
		for (uint32_t queueFamily : uniqueQueueFamilies) {
			queue_lockers.emplace(queueFamily, std::make_shared<std::mutex>());

			VkDeviceQueueCreateInfo queueCreateInfo{};
			queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueCreateInfo.queueFamilyIndex = queueFamily;
			queueCreateInfo.queueCount = 1;
			queueCreateInfo.pQueuePriorities = &queuePriority;
			queueCreateInfos.push_back(queueCreateInfo);
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
		createInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
		createInfo.ppEnabledExtensionNames = deviceExtensions.data();

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
		queues[QUEUE_GRAPHICS].queue = graphics_queue;
		queues[QUEUE_GRAPHICS].locker = queue_lockers[queue_family_indices.graphics_family.value()];

		queues[QUEUE_COMPUTE].queue = compute_queue;
		queues[QUEUE_COMPUTE].locker = queue_lockers[queue_family_indices.compute_family.value()];

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
			}

			if (!indices.compute_family.has_value() && queueFamily.queueCount > 0
				&& queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				indices.compute_family = i;
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

	void VulkanRHI::createFramebuffers() {
		swapChainFramebuffers.resize(swapChainImageViews.size());

		for (size_t i = 0; i < swapChainImageViews.size(); i++) {
			std::array<VkImageView, 3> attachments = {
				colorImageView,
				depth_image_view,
				swapChainImageViews[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = renderPass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_swapchain_extent.width;
			framebufferInfo.height = m_swapchain_extent.height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_device, &framebufferInfo, nullptr, &swapChainFramebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}
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
		createFramebuffers();
	}

	void VulkanRHI::cleanupSwapChain() {
		vkDeviceWaitIdle(m_device);
		vkWaitForFences(m_device, MAX_FRAMES_IN_FLIGHT, m_is_frame_in_flight_fences.data(), VK_TRUE, UINT64_MAX);
		vkDestroyImageView(m_device, depth_image_view, nullptr);
		vkDestroyImage(m_device, depthImage, nullptr);
		vkFreeMemory(m_device, depthImageMemory, nullptr);

		vkDestroyImageView(m_device, colorImageView, nullptr);
		vkDestroyImage(m_device, colorImage, nullptr);
		vkFreeMemory(m_device, colorImageMemory, nullptr);

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

	void VulkanRHI::submitCommandLists(CommandList& cmd)
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

	void VulkanRHI::bindViewports(CommandList& cmd_list, uint32_t viewport_count, Viewport* viewport)
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
		internal_swapchain->locker.unlock();

		commandlist.prev_swapchains.push_back(*swapchain);
		VkRenderingInfo info{};
		info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		info.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
		info.renderArea.offset.x = 0;
		info.renderArea.offset.y = 0;
		info.renderArea.extent.width = std::min(swapchain->desc.width, internal_swapchain->swapChainExtent.width);
		info.renderArea.extent.height = std::min(swapchain->desc.height, internal_swapchain->swapChainExtent.height);
		info.layerCount = 1;

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

		vkCmdBeginRendering(commandlist.getCommandBuffer(), &info);
	}

	void VulkanRHI::endRenderPass(CommandList& cmd_list)
	{
		CommandList_Vulkan& commandlist = getCommandList(cmd_list);
		vkCmdEndRendering(commandlist.getCommandBuffer());
	}

	void VulkanRHI::submitRender(std::function<void()> update_pass_after_recreate_swap_chain)
	{

		if (vkEndCommandBuffer(m_command_buffers[m_current_frame_index]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { imageAvailableSemaphores[m_current_frame_index] };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &m_command_buffers[m_current_frame_index];

		VkSemaphore signalSemaphores[] = { renderFinishedSemaphores[m_current_frame_index] };
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

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

		bool extensionsSupported = checkDeviceExtensionSupport(device);

		VkPhysicalDeviceFeatures supportedFeatures;
		vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

		return  extensionsSupported && supportedFeatures.samplerAnisotropy;
	}

	bool VulkanRHI::checkDeviceExtensionSupport(VkPhysicalDevice device) {
		uint32_t extensionCount;
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

		std::vector<VkExtensionProperties> availableExtensions(extensionCount);
		vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

		std::set<std::string> requiredExtensions(deviceExtensions.begin(), deviceExtensions.end());

		for (const auto& extension : availableExtensions) {
			requiredExtensions.erase(extension.extensionName);
		}

		return requiredExtensions.empty();
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

	VulkanRHI::CommandList_Vulkan& VulkanRHI::getCommandList(CommandList cmd)
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

}