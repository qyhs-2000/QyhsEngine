#pragma once
#include "function/render/rhi/rhi.h"
namespace qyhs::texturehelper
{
	bool createTexture(Texture& texture, const void* data, uint32_t width, uint32_t height, Format format = Format::R8G8B8A8_UNORM, Swizzle swizzle = {});
}