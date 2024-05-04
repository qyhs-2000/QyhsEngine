#pragma once
#include <vulkan/vulkan.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
namespace QYHS
{
	class Texture
	{

	};

	class Texture2D :public Texture
	{


	public:
		void fromBuffer(unsigned char * buffer,unsigned int buffer_size,VkFormat format,unsigned int width,unsigned int weight, VkDevice device = VK_NULL_HANDLE);
	};
}