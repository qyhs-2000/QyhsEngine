#include "Texture.h"
#include <assert.h>
#include <cmath>
#include <exception>
#include <stdexcept>

//not done yet
void QYHS::Texture2D::fromBuffer(unsigned char* buffer, unsigned int buffer_size, VkFormat format, unsigned int width, unsigned int height,VkDevice device)
{
	assert(buffer);
	uint32_t mip_levels = std::floor(std::log2(width > height ? width : height));
	
	VkBufferCreateInfo buffer_create_info{};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = static_cast<VkDeviceSize>(buffer_size);
	buffer_create_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	VkBuffer stage_buffer;
	

	//if (vkCreateBuffer(device, &buffer_create_info, nullptr, &stage_buffer) != VK_SUCCESS)
	//{
	//	//throw std::runtime_error("failed to create buffer!");
	//}
}
