#include "graphics.h"
#include <cassert>
namespace qyhs
{

}

qyhs::graphics::Sampler::~Sampler()
{
}


namespace qyhs::graphics
{

	constexpr bool isFormatBlockCompressed(Format format)
	{
		switch (format)
		{
		case Format::BC1_UNORM:
		case Format::BC1_UNORM_SRGB:
		case Format::BC2_UNORM:
		case Format::BC2_UNORM_SRGB:
		case Format::BC3_UNORM:
		case Format::BC3_UNORM_SRGB:
		case Format::BC4_UNORM:
		case Format::BC4_SNORM:
		case Format::BC5_UNORM:
		case Format::BC5_SNORM:
		case Format::BC6H_UF16:
		case Format::BC6H_SF16:
		case Format::BC7_UNORM:
		case Format::BC7_UNORM_SRGB:
			return true;
		default:
			return false;
		}
	}

}

const uint32_t qyhs::graphics::getFormatStride(Format format)
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

const uint32_t qyhs::graphics::getFormatBlockSize(Format format)
{
	if (isFormatBlockCompressed(format))
	{
		return 4u;
	}
	return 1u;
}

uint32_t qyhs::graphics::getMipCount(uint32_t width, uint32_t height, uint32_t depth, uint32_t min_dimension, uint32_t required_alignment)
{
	uint32_t mips = 1;
	while (width > min_dimension && height > min_dimension && depth > min_dimension)
	{
		width = std::max(min_dimension, width >> 1u);
		height = std::max(min_dimension, height >> 1u);
		depth = std::max(min_dimension, depth >> 1u);

		if (alignTo(width, required_alignment) != width
			|| alignTo(height, required_alignment)
			|| alignTo(depth, required_alignment))
		{
			break;
		}
		++mips;
	}
	return mips;
}
