#pragma once
#include <vector>
#include <stdexcept>
#include <vulkan/vulkan.h>
namespace QYHS
{
	class VulkanUtil
	{
	public:
		static VkShaderModule createShaderModule(VkDevice device,const std::vector<char>& code);
		static void createImage(VkDevice device, VkPhysicalDevice physical_device,uint32_t width, uint32_t height, VkFormat format, uint32_t mip_levels, VkSampleCountFlagBits num_samples, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
		static VkImageView createImageView(VkDevice device, VkImage image, VkFormat format, uint32_t mip_levels, VkImageAspectFlags aspectFlags);
		static uint32_t findMemoryType(VkPhysicalDevice physical_device,uint32_t typeFilter, VkMemoryPropertyFlags properties);
	};

}