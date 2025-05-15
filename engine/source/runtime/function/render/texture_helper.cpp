#include "texture_helper.h"

namespace qyhs::texturehelper
{
	bool createTexture(Texture& texture, const void* data, uint32_t width, uint32_t height, Format format, Swizzle swizzle)
	{
		if (data == nullptr)
		{
			return false;
		}
		RHI* rhi = rhi::getRHI();

		TextureDesc desc;
		desc.width = width;
		desc.height = height;
		desc.mip_levels = 1;
		desc.array_size = 1;
		desc.format = format;
		desc.sample_count = 1;
		desc.bind_flags = BindFlag::SHADER_RESOURCE;
		desc.swizzle = swizzle;

		SubresourceData InitData;
		InitData.data_ptr = data;
		InitData.row_pitch = width * getFormatStride(format) / getFormatBlockSize(format);

		return rhi->createTexture(&desc, &texture,&InitData);
	}

}
