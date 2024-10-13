#pragma once
#include <vector>
#include <stdexcept>
#include <vulkan/vulkan.h>
#include "function/render/rhi/rhi.h"
#include <unordered_map>
#include "vulkanmemoryallocator/include/vk_mem_alloc.h"
#include "function/render/render_type.h"
namespace QYHS
{
	

	class VulkanUtils
	{
	public:
		static VkShaderModule createShaderModule(VkDevice device,const std::vector<char>& code);
		static void createImage(VkPhysicalDevice physical_device, VkDevice device,uint32_t width, uint32_t height, VkFormat format, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format,VkImageViewType image_view_type,uint32_t layer_count, uint32_t mip_levels, VkImageAspectFlags aspectFlags);
		static uint32_t findMemoryType(VkPhysicalDevice physical_device,uint32_t typeFilter, VkMemoryPropertyFlags properties);
		static void createBuffer(VkPhysicalDevice physical_device, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
		static void transitionImageLayout(RHI* vulkan_rhi, VkImage image, VkFormat format, uint32_t layer_count,uint32_t mip_levels, VkImageLayout oldLayout, VkImageLayout newLayout);
		static void copyBufferToImage(RHI* vulkan_rhi, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height,uint32_t layer_count);
		static void generateMipmaps(RHI* vulkan_rhi, VkImage image, VkFormat format, uint32_t tex_width, uint32_t tex_height, uint32_t mip_levels);
		static void createTextureImageView(VkDevice device, VkImage image, VkImageView &image_view, uint32_t mip_levels);
		static void copyBuffer(RHI* vulkan_rhi,VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
		static VkSampler& getOrCreateMipMapSampler(VkPhysicalDevice physical_device, VkDevice device, uint32_t mip_levels);
		static std::unordered_map<uint32_t, VkSampler> m_mipmap_sampler_map;
		static void createCubeMap(RHI* rhi, VkImage& image, VkImageView& image_view, VmaAllocation& allocation, uint32_t width, uint32_t height, std::array<void*, 6> pixels, PIXEL_FORMAT texture_image_format, uint32_t miplevels);
		static VkSampler getOrCreateNearestSampler(VkPhysicalDevice physical_device, VkDevice device);
	private:
		static VkSampler m_nearest_sampler;
	};

}