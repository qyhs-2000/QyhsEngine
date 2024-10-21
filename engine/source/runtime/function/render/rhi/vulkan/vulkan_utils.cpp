#include "vulkan_utils.h"
#include "vulkan_rhi.h"

#include <algorithm>
#include <cassert>
#include <cmath>
#include <cstring>
#include <stdexcept>

namespace QYHS
{
	VkSampler VulkanUtils::m_nearest_sampler = VK_NULL_HANDLE;

	std::unordered_map<uint32_t, VkSampler> VulkanUtils::m_mipmap_sampler_map;
	VkShaderModule VulkanUtils::createShaderModule(VkDevice device, const std::vector<char>& code)
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

	void VulkanUtils::createBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {
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

	VkImageView VulkanUtils::createImageView(VkDevice device, VkImage image, VkFormat format,VkImageViewType image_view_type,uint32_t layer_count, uint32_t mip_levels, VkImageAspectFlags aspectFlags) {
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

	void VulkanUtils::transitionImageLayout(RHI* rhi, VkImage image, VkFormat format,uint32_t layer_count, uint32_t mip_levels, VkImageLayout oldLayout, VkImageLayout newLayout) {
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

	void VulkanUtils::copyBufferToImage(RHI* rhi, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,uint32_t layer_count) {
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

	void VulkanUtils::createTextureImageView(VkDevice device, VkImage image, VkImageView &image_view, uint32_t mip_levels) {
		image_view = createImageView(device, image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_VIEW_TYPE_2D,1,mip_levels, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	void VulkanUtils::copyBuffer(RHI* rhi, VkBuffer srcBuffer, VkBuffer dstBuffer,VkDeviceSize src_offset,VkDeviceSize dst_offset, VkDeviceSize size)
	{
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		VkCommandBuffer commandBuffer = vulkan_rhi->beginSingleTimeCommands();

		VkBufferCopy copyRegion{src_offset,dst_offset,size};
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		vulkan_rhi->endSingleTimeCommands(commandBuffer);

	}

	void VulkanUtils::generateMipmaps(RHI* rhi, VkImage image, VkFormat format, uint32_t tex_width, uint32_t tex_height, uint32_t mip_levels)
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

	VkSampler& VulkanUtils::getOrCreateMipMapSampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t mip_levels) {
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

	void VulkanUtils::createCubeMap(RHI* rhi, VkImage& image, VkImageView& image_view, VmaAllocation& allocation, uint32_t width, uint32_t height, std::array<void*, 6> pixels, PIXEL_FORMAT texture_image_format, uint32_t miplevels)
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
        cube_map_image_create_info.samples     = VK_SAMPLE_COUNT_1_BIT;
        cube_map_image_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		VmaAllocationCreateInfo allocate_info = {};
		allocate_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		vmaCreateImage(static_cast<VulkanRHI*>(rhi)->m_assets_allocator, 
			&cube_map_image_create_info, &allocate_info, &image, &allocation, nullptr);

		VkBuffer storage_buffer;
		VkDeviceMemory storage_buffer_memory;
		VulkanRHI* vulkan_rhi = static_cast<VulkanRHI*>(rhi);
		createBuffer(vulkan_rhi->getPhysicalDevice(), vulkan_rhi->getDevice(), cube_byte_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, storage_buffer,storage_buffer_memory);

		void* data;
		vkMapMemory(vulkan_rhi->getDevice(), storage_buffer_memory, 0, cube_byte_size, 0, &data);
		for (int i = 0; i < 6; ++i)
		{
			memcpy((void*)(static_cast<char*>(data) + texture_byte_size * i), pixels[i], static_cast<size_t>(texture_byte_size));

		}
		vkUnmapMemory(vulkan_rhi->getDevice(), storage_buffer_memory);
		transitionImageLayout(rhi, image, vulkan_image_format,6, miplevels, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
		copyBufferToImage(rhi, storage_buffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height),6);
		transitionImageLayout(rhi, image, vulkan_image_format,6, miplevels, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
		vkDestroyBuffer(vulkan_rhi->getDevice(), storage_buffer, nullptr);
		vkFreeMemory(vulkan_rhi->getDevice(), storage_buffer_memory, nullptr);

		image_view = createImageView(vulkan_rhi->getDevice(), image, vulkan_image_format,VK_IMAGE_VIEW_TYPE_CUBE,6, miplevels, VK_IMAGE_ASPECT_COLOR_BIT);
	}

	VkSampler VulkanUtils::getOrCreateNearestSampler(VkPhysicalDevice physical_device, VkDevice device)
	{
		if (m_nearest_sampler == VK_NULL_HANDLE)
		{
			VkPhysicalDeviceProperties physical_property;
			vkGetPhysicalDeviceProperties(physical_device, &physical_property);

			VkSamplerCreateInfo create_info{};
			create_info.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
            create_info.magFilter               = VK_FILTER_NEAREST;
            create_info.minFilter               = VK_FILTER_NEAREST;
            create_info.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_NEAREST;
            create_info.addressModeU            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            create_info.addressModeV            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            create_info.addressModeW            = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
            create_info.mipLodBias              = 0.0f;
            create_info.anisotropyEnable        = VK_FALSE;
            create_info.maxAnisotropy           = physical_property.limits.maxSamplerAnisotropy; // close :1.0f
            create_info.compareEnable           = VK_FALSE;
            create_info.compareOp               = VK_COMPARE_OP_ALWAYS;
            create_info.minLod                  = 0.0f;
            create_info.maxLod                  = 8.0f; // todo: m_irradiance_texture_miplevels
            create_info.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
            create_info.unnormalizedCoordinates = VK_FALSE;

			vkCreateSampler(device, &create_info, nullptr, &m_nearest_sampler);
		}
		return m_nearest_sampler;
	}



	void VulkanUtils::createImage(VkPhysicalDevice physical_device,
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

	uint32_t VulkanUtils::findMemoryType(VkPhysicalDevice physical_device, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical_device, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}
}
