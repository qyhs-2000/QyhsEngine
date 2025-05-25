#pragma once
#include <cstdint>
#include <limits>
#include <memory>
#include "core/common_include.h"
#include <string>
#include <vector>
namespace qyhs::graphics
{

	template<typename T>
	constexpr T alignTo(T value, T alignment)
	{
		return ((value + alignment - T(1)) / alignment) * alignment;
	}

	enum class ShaderModel : uint8_t
	{
		SM_5_0,
		SM_6_0,
		SM_6_1,
		SM_6_2,
		SM_6_3,
		SM_6_4,
		SM_6_5,
		SM_6_6,
		SM_6_7,
	};

	enum class IndexBufferFormat : uint8_t
	{
		UINT16,
		UINT32,
	};

	enum class SubresourceType : uint8_t
	{
		SRV, // shader resource view
		UAV, // unordered access view
		RTV, // render target view
		DSV, // depth stencil view
	};

	enum class Format : uint8_t
	{
		UNKNOWN,

		R32G32B32A32_FLOAT,
		R32G32B32A32_UINT,
		R32G32B32A32_SINT,

		R32G32B32_FLOAT,
		R32G32B32_UINT,
		R32G32B32_SINT,

		R16G16B16A16_FLOAT,
		R16G16B16A16_UNORM,
		R16G16B16A16_UINT,
		R16G16B16A16_SNORM,
		R16G16B16A16_SINT,

		R32G32_FLOAT,
		R32G32_UINT,
		R32G32_SINT,
		D32_FLOAT_S8X24_UINT,	// depth (32-bit) + stencil (8-bit) | SRV: R32_FLOAT (default or depth aspect), R8_UINT (stencil aspect)

		R10G10B10A2_UNORM,
		R10G10B10A2_UINT,
		R11G11B10_FLOAT,
		R8G8B8A8_UNORM,
		R8G8B8A8_UNORM_SRGB,
		R8G8B8A8_UINT,
		R8G8B8A8_SNORM,
		R8G8B8A8_SINT,
		B8G8R8A8_UNORM,
		B8G8R8A8_UNORM_SRGB,
		R16G16_FLOAT,
		R16G16_UNORM,
		R16G16_UINT,
		R16G16_SNORM,
		R16G16_SINT,
		D32_FLOAT,				// depth (32-bit) | SRV: R32_FLOAT
		R32_FLOAT,
		R32_UINT,
		R32_SINT, 
		D24_UNORM_S8_UINT,		// depth (24-bit) + stencil (8-bit) | SRV: R24_INTERNAL (default or depth aspect), R8_UINT (stencil aspect)
		R9G9B9E5_SHAREDEXP,

		R8G8_UNORM,
		R8G8_UINT,
		R8G8_SNORM,
		R8G8_SINT,
		R16_FLOAT,
		D16_UNORM,				// depth (16-bit) | SRV: R16_UNORM
		R16_UNORM,
		R16_UINT,
		R16_SNORM,
		R16_SINT,

		R8_UNORM,
		R8_UINT,
		R8_SNORM,
		R8_SINT,

		// Formats that are not usable in render pass must be below because formats in render pass must be encodable as 6 bits:

		BC1_UNORM,			// Three color channels (5 bits:6 bits:5 bits), with 0 or 1 bit(s) of alpha
		BC1_UNORM_SRGB,		// Three color channels (5 bits:6 bits:5 bits), with 0 or 1 bit(s) of alpha
		BC2_UNORM,			// Three color channels (5 bits:6 bits:5 bits), with 4 bits of alpha
		BC2_UNORM_SRGB,		// Three color channels (5 bits:6 bits:5 bits), with 4 bits of alpha
		BC3_UNORM,			// Three color channels (5 bits:6 bits:5 bits) with 8 bits of alpha
		BC3_UNORM_SRGB,		// Three color channels (5 bits:6 bits:5 bits) with 8 bits of alpha
		BC4_UNORM,			// One color channel (8 bits)
		BC4_SNORM,			// One color channel (8 bits)
		BC5_UNORM,			// Two color channels (8 bits:8 bits)
		BC5_SNORM,			// Two color channels (8 bits:8 bits)
		BC6H_UF16,			// Three color channels (16 bits:16 bits:16 bits) in "half" floating point
		BC6H_SF16,			// Three color channels (16 bits:16 bits:16 bits) in "half" floating point
		BC7_UNORM,			// Three color channels (4 to 7 bits per channel) with 0 to 8 bits of alpha
		BC7_UNORM_SRGB,		// Three color channels (4 to 7 bits per channel) with 0 to 8 bits of alpha

		NV12,				// video YUV420; SRV Luminance aspect: R8_UNORM, SRV Chrominance aspect: R8G8_UNORM
	};

	constexpr bool IsFormatUnorm(Format format)
	{
		switch (format)
		{
		case Format::R16G16B16A16_UNORM:
		case Format::R10G10B10A2_UNORM:
		case Format::R8G8B8A8_UNORM:
		case Format::R8G8B8A8_UNORM_SRGB:
		case Format::B8G8R8A8_UNORM:
		case Format::B8G8R8A8_UNORM_SRGB:
		case Format::R16G16_UNORM:
		case Format::D24_UNORM_S8_UINT:
		case Format::R8G8_UNORM:
		case Format::D16_UNORM:
		case Format::R16_UNORM:
		case Format::R8_UNORM:
		case Format::BC1_UNORM:
		case Format::BC1_UNORM_SRGB:
		case Format::BC2_UNORM:
		case Format::BC2_UNORM_SRGB:
		case Format::BC3_UNORM:
		case Format::BC3_UNORM_SRGB:
		case Format::BC4_UNORM:
		case Format::BC5_UNORM:
		case Format::BC7_UNORM:
		case Format::BC7_UNORM_SRGB:
			return true;
		default:
			return false;
		}
	}

	enum class ImageAspect : uint8_t
	{
		COLOR,
		DEPTH,
		STENCIL,
		LUMINANCE,
		CHROMINANCE,
	};

	union ClearValue
	{
		float color[4];
		struct ClearDepthStencil
		{
			float depth;
			uint32_t stencil;
		} depth_stencil;
	};

	struct Rect
	{
		int32_t left = 0;
		int32_t right = 0;
		int32_t top = 0;
		int32_t bottom = 0;
	};

	enum class SamplerBorderColor : uint8_t
	{
		TRANSPARENT_BLACK,
		OPAQUE_BLACK,
		OPAQUE_WHITE,
	};

	enum StencilRefMode
	{
		STENCILREFMODE_ENGINE,
		STENCILREFMODE_USER,
		STENCILREFMODE_ALL,
		STENCILREFMODE_COUNT
	};

	enum class ColorSpace : uint8_t
	{
		SRGB,			// SDR color space (8 or 10 bits per channel)
		HDR10_ST2084,	// HDR10 color space (10 bits per channel)
		HDR_LINEAR,		// HDR color space (16 bits per channel)
	};

	enum class ComparisonFunc : uint8_t
	{
		NEVER,
		LESS,
		EQUAL,
		LESS_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_EQUAL,
		ALWAYS,
	};
	enum class DepthWriteMask : uint8_t
	{
		ZERO,	// Disables depth write
		ALL,	// Enables depth write
	};
	enum class StencilOp : uint8_t
	{
		KEEP,
		ZERO,
		REPLACE,
		INCR_SAT,
		DECR_SAT,
		INVERT,
		INCR,
		DECR,
	};
	enum class Blend : uint8_t
	{
		ZERO,
		ONE,
		SRC_COLOR,
		INV_SRC_COLOR,
		SRC_ALPHA,
		INV_SRC_ALPHA,
		DEST_ALPHA,
		INV_DEST_ALPHA,
		DEST_COLOR,
		INV_DEST_COLOR,
		SRC_ALPHA_SAT,
		BLEND_FACTOR,
		INV_BLEND_FACTOR,
		SRC1_COLOR,
		INV_SRC1_COLOR,
		SRC1_ALPHA,
		INV_SRC1_ALPHA,
	}; 
	enum class BlendOp : uint8_t
	{
		ADD,
		SUBTRACT,
		REV_SUBTRACT,
		MIN,
		MAX,
	};
	enum class FillMode : uint8_t
	{
		WIREFRAME,
		SOLID,
	};
	enum class CullMode : uint8_t
	{
		NONE = 0,
		FRONT,
		BACK,
		COUNT
	};
	enum class InputClassification : uint8_t
	{
		PER_VERTEX_DATA,
		PER_INSTANCE_DATA,
	};

	enum class ComponentSwizzle : uint8_t
	{
		R,
		G,
		B,
		A,
		ZERO,
		ONE,
	};

	struct Swizzle
	{
		ComponentSwizzle r = ComponentSwizzle::R;
		ComponentSwizzle g = ComponentSwizzle::G;
		ComponentSwizzle b = ComponentSwizzle::B;
		ComponentSwizzle a = ComponentSwizzle::A;
	};

	struct RasterizerState
	{
		FillMode fill_mode = FillMode::SOLID;
		CullMode cull_mode = CullMode::NONE;
		bool front_counter_clockwise = false;
		int32_t depth_bias = 0;
		float depth_bias_clamp = 0;
		float slope_scaled_depth_bias = 0;
		bool depth_clip_enable = false;
		bool multisample_enable = false;
		bool antialiased_line_enable = false;
		bool conservative_rasterization_enable = false;
		uint32_t forced_sample_count = 0;
	};

	struct InputLayout
	{
		static const uint32_t APPEND_ALIGNED_ELEMENT = ~0u; // automatically figure out AlignedByteOffset depending on Format
		struct Element
		{
			std::string semantic_name;
			uint32_t semantic_index = 0;
			Format format = Format::UNKNOWN;
			uint32_t input_slot = 0;
			uint32_t aligned_byte_offset = APPEND_ALIGNED_ELEMENT;
			InputClassification input_slot_class = InputClassification::PER_VERTEX_DATA;
		};
		std::vector<Element> elements;
		
	};

	enum class ColorWrite
	{
		DISABLE = 0,
		ENABLE_RED = 1 << 0,
		ENABLE_GREEN = 1 << 1,
		ENABLE_BLUE = 1 << 2,
		ENABLE_ALPHA = 1 << 3,
		ENABLE_ALL = ~0,
	};

	enum class ShaderFormat :uint8_t
	{
		NONE,
		HLSL5,
		HLSL6,
		HLSL6_XS,
		SPIRV
	};

	enum class BindFlag
	{
		NONE = 0,
		VERTEX_BUFFER = 1 << 0,
		INDEX_BUFFER = 1 << 1,
		CONSTANT_BUFFER = 1 << 2,
		SHADER_RESOURCE = 1 << 3,
		RENDER_TARGET = 1 << 4,
		DEPTH_STENCIL = 1 << 5,
		UNORDERED_ACCESS = 1 << 6,
		SHADING_RATE = 1 << 7,
	};

	enum class Usage : uint8_t
	{
		DEFAULT,	// CPU no access, GPU read/write
		UPLOAD,	    // CPU write, GPU read
		READBACK,	// CPU read, GPU write
	};

	enum class GraphicsDeviceCapability
	{
		NONE = 0,
		SAMPLER_MINMAX = 1 << 0,
		CONSERVATIVE_RASTERIZATION = 1 << 1,
		DEPTH_BOUNDS_TEST = 1 << 2,
		RAYTRACING = 1 << 3,
		VARIABLE_RATE_SHADING = 1 << 4,
		MESH_SHADER = 1 << 5
	};

	enum class ResourceMiscFlag
	{
		NONE = 0,
		TRANSIENT_ATTACHMENT = 1 << 0,
		TEXTURE_CUBE = 1 << 1,
		NO_DEFAULT_DESCRIPTORS = 1 << 2,
		BUFFER_RAW = 1 << 3,
		BUFFER_STRUCTURED = 1 << 4,
		TYPED_FORMAT_CASTING = 1<<5
	};

	enum class ShaderStage:uint8_t
	{
		NONE,
		AMPLIFICATION_SHADER,			//task stage in vulkan
		MESH_SHADER,
		VERTEX_SHADER,
		HULL_SHADER,
		DOMAIN_SHADER,
		GEOMETRY_SHADER,
		PIXEL_SHADER,					//fragment shader in vulkan
		COMPUTE_SHADER,
		LIBRARY,
		COUNT
	};

	enum class RenderPassFlags
	{
		NONE = 0,
		ALLOW_UAV_WRITES = 1 << 0,
		SUSPENDING = 1 << 1,
		RESUMING = 1 << 2,
	};

	enum class ResourceState
	{
		// Common resource states:
		UNDEFINED = 0,						// invalid state (don't preserve contents)
		SHADER_RESOURCE = 1 << 0,			// shader resource, read only
		SHADER_RESOURCE_COMPUTE = 1 << 1,	// shader resource, read only, non-pixel shader
		UNORDERED_ACCESS = 1 << 2,			// shader resource, write enabled
		COPY_SRC = 1 << 3,					// copy from
		COPY_DST = 1 << 4,					// copy to

		// Texture specific resource states:
		RENDERTARGET = 1 << 5,				// render target, write enabled
		DEPTHSTENCIL = 1 << 6,				// depth stencil, write enabled
		DEPTHSTENCIL_READONLY = 1 << 7,		// depth stencil, read only
		SHADING_RATE_SOURCE = 1 << 8,		// shading rate control per tile

		// GPUBuffer specific resource states:
		VERTEX_BUFFER = 1 << 9,				// vertex buffer, read only
		INDEX_BUFFER = 1 << 10,				// index buffer, read only
		CONSTANT_BUFFER = 1 << 11,			// constant buffer, read only
		INDIRECT_ARGUMENT = 1 << 12,		// argument buffer to DrawIndirect() or DispatchIndirect()
		RAYTRACING_ACCELERATION_STRUCTURE = 1 << 13, // acceleration structure storage or scratch
		PREDICATION = 1 << 14,				// storage for predication comparison value

		// Other:
		VIDEO_DECODE_SRC = 1 << 15,			// video decode operation source (bitstream buffer or DPB texture)
		VIDEO_DECODE_DST = 1 << 16,			// video decode operation destination DPB texture
	};
	constexpr IndexBufferFormat getIndexBufferFormat(uint32_t vertex_count)
	{
		return vertex_count > 65536 ? IndexBufferFormat::UINT32 : IndexBufferFormat::UINT16;
	}

	const uint32_t getFormatStride(Format format);
	const uint32_t getFormatBlockSize(Format format);
	constexpr bool isFormatBlockCompressed(Format format);
	struct SubresourceData
	{
		const void* data_ptr = nullptr;	// pointer to the beginning of the subresource data (pointer to beginning of resource + subresource offset)
		uint32_t row_pitch = 0;			// bytes between two rows of a texture (2D and 3D textures)
		uint32_t slice_pitch = 0;		// bytes between two depth slices of a texture (3D textures only)
	};

	struct BlendState
	{
		bool alpha_to_coverage_enable{ false };
		bool independent_blend_enable{ false };
		struct RenderTargetBlendState
		{
			bool blend_enable = false;
			ColorWrite render_target_write_mask = ColorWrite::ENABLE_ALL;
			Blend src_blend = Blend::SRC_ALPHA;
			Blend dest_blend = Blend::INV_SRC_ALPHA;
			BlendOp blend_op = BlendOp::ADD;
			Blend src_blend_alpha = Blend::ONE;
			Blend dest_blend_alpha = Blend::ONE;
			BlendOp blend_op_alpha = BlendOp::ADD;

		} render_targets[8];
	};

	struct GraphicsChild
	{
		uint64_t handle = 0;
		std::shared_ptr<void> internal_state;
		bool isValid() const{ return internal_state != nullptr; }
	};

	struct GPUResource:GraphicsChild
	{
		enum class Type : uint8_t
		{
			BUFFER,
			TEXTURE,
			RAYTRACING_ACCELERATION_STRUCTURE,
			UNKNOWN_TYPE,
		} type = Type::UNKNOWN_TYPE;
		uint64_t handle = 0;
		uint32_t size = 0;
		void* mapped_data = nullptr;	
		size_t mapped_size = 0;	
		constexpr bool isTexture() const
		{
			return type == Type::TEXTURE;
		}
		constexpr bool isBuffer() const
		{
			return type == Type::BUFFER;
		}
	};

	struct TextureDesc
	{
		BindFlag bind_flags;
		ResourceMiscFlag misc_flags = ResourceMiscFlag::NONE;
		Format format;
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t depth = 1;
		uint32_t array_size = 1;
		uint32_t mip_levels = 0;
		Swizzle swizzle;
		ClearValue clear = {  };
		ResourceState layout = ResourceState::SHADER_RESOURCE;
		enum class Type
		{
			TEXTURE_1D,
			TEXTURE_2D,
			TEXTURE_3D
		}type = Type::TEXTURE_2D;
		uint32_t sample_count = 1;
		Usage usage = Usage::DEFAULT;
	};

	struct Texture :public GPUResource
	{
		TextureDesc desc;

	};

	struct RenderPassImage
	{
		enum class LoadOp
		{
			LOAD,
			CLEAR,
			DONT_CARE
		} loadop = LoadOp::LOAD;
		enum class StoreOp
		{
			STORE,
			DONT_CARE
		} storeop = StoreOp::STORE;
		enum class Type
		{
			RENDERTARGET,
			DEPTH_STENCIL,
			RESOLVE, // resolve render target (color)
			RESOLVE_DEPTH,
			SHADING_RATE_SOURCE
		} type = Type::RENDERTARGET;
		const Texture* texture = nullptr;
		ResourceState layout_before = ResourceState::UNDEFINED;
		ResourceState layout = ResourceState::UNDEFINED;
		ResourceState layout_after = ResourceState::UNDEFINED;
		int subresource = -1;
		static RenderPassImage renderTarget(const Texture* texture, LoadOp loadOp = LoadOp::LOAD, StoreOp storeOp = StoreOp::STORE,
			ResourceState layout_before = ResourceState::SHADER_RESOURCE,ResourceState layout_after = ResourceState::SHADER_RESOURCE)
		{
			RenderPassImage render_pass_image;
			render_pass_image.type = Type::RENDERTARGET;
			render_pass_image.texture = texture;
			render_pass_image.loadop = loadOp;
			render_pass_image.storeop = storeOp;
			render_pass_image.layout_before = layout_before;
			render_pass_image.layout_after = layout_after;
			return render_pass_image;
		}
		static RenderPassImage depthStencil(const Texture* texture, LoadOp loadOp = LoadOp::LOAD, StoreOp storeOp = StoreOp::STORE,
			ResourceState layout_before = ResourceState::DEPTHSTENCIL,ResourceState layout = ResourceState::DEPTHSTENCIL,ResourceState layout_after = ResourceState::DEPTHSTENCIL,int subresource_dsv = -1)
		{
			RenderPassImage render_pass_image;
			render_pass_image.type = Type::DEPTH_STENCIL;
			render_pass_image.texture = texture;
			render_pass_image.loadop = loadOp;
			render_pass_image.storeop = storeOp;
			render_pass_image.layout_before = layout_before;
			render_pass_image.layout_after = layout_after;
			render_pass_image.subresource = subresource_dsv;
			return render_pass_image;
		}

		static RenderPassImage resolve(const Texture* texture, ResourceState layout_before = ResourceState::SHADER_RESOURCE, ResourceState layout_after = ResourceState::SHADER_RESOURCE, int subresource_srv = -1)
		{
			RenderPassImage image;
			image.type = Type::RESOLVE;
			image.texture = texture;
			image.layout_before = layout_before;
			image.layout = ResourceState::COPY_DST;
			image.layout_after = layout_after;
			image.subresource = subresource_srv;
			return image;
		}
	};

	struct DepthStencilState
	{
		bool depth_enable = false;
		DepthWriteMask depth_write_mask = DepthWriteMask::ZERO;
		ComparisonFunc depth_func = ComparisonFunc::NEVER;
		bool stencil_enable = false;
		uint8_t stencil_read_mask = 0xff;
		uint8_t stencil_write_mask = 0xff;

		struct DepthStencilOp
		{
			StencilOp stencil_fail_op = StencilOp::KEEP;
			StencilOp stencil_depth_fail_op = StencilOp::KEEP;
			StencilOp stencil_pass_op = StencilOp::KEEP;
			ComparisonFunc stencil_func = ComparisonFunc::NEVER;
		};
		DepthStencilOp front_face;
		DepthStencilOp back_face;
		bool depth_bounds_test_enable = false;
	};

	enum class PrimitiveTopology :uint8_t
	{
		TRIANGLE_STRIP,
		TRIANGLE_LIST,
		POINT_LIST,
		LINE_LIST,
		LINE_STRIP,
		PATCH_LIST
	};

	struct Shader:public GraphicsChild
	{
		ShaderStage stage = ShaderStage::COUNT;
	};

	struct PipelineStateDesc
	{
		const Shader* vertex_shader = nullptr;
		const Shader* fragment_shader = nullptr;
		const Shader* hull_shader = nullptr;
		const Shader* domain_shader= nullptr;
		const Shader* geometry_shader = nullptr;
		const Shader* mesh_shader = nullptr;
		const Shader* amplification_shader = nullptr;
		const RasterizerState*	rasterizer_state = nullptr;
		const BlendState* blend_state = nullptr;
		const DepthStencilState* depth_stencil_state = nullptr;
		const InputLayout* input_layout = nullptr;
		PrimitiveTopology primitive_topology;
		uint32_t sample_mask = 0xFFFFFFFF;
		uint32_t patch_control_points = 3;
	};

	struct PipelineState :public GraphicsChild
	{
		PipelineStateDesc desc;
	};

	struct GPUBufferDesc
	{
		uint64_t size = 0;
		uint32_t stride = 0;  //only for structured buffer types
		BindFlag bind_flags = BindFlag::NONE;
		Usage usage = Usage::DEFAULT;
		ResourceMiscFlag misc_flags = ResourceMiscFlag::NONE;
		Format format = Format::UNKNOWN;
	};

	struct GPUBuffer :public GPUResource
	{
		GPUBufferDesc desc;
				
	};

	uint32_t getMipCount(uint32_t width, uint32_t height, uint32_t depth = 1u, uint32_t min_dimension = 1u, uint32_t required_alignment = 1u);

	class GraphicsDeviceChild
	{
	public:
		virtual ~GraphicsDeviceChild() = default;
	};

	enum class TextureAddressMode : uint8_t
	{
		WRAP,
		MIRROR,
		CLAMP,
		BORDER,
		MIRROR_ONCE,
	};

	enum class Filter : uint8_t
	{
		MIN_MAG_MIP_POINT,
		MIN_MAG_POINT_MIP_LINEAR,
		MIN_POINT_MAG_LINEAR_MIP_POINT,
		MIN_POINT_MAG_MIP_LINEAR,
		MIN_LINEAR_MAG_MIP_POINT,
		MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MIN_MAG_LINEAR_MIP_POINT,
		MIN_MAG_MIP_LINEAR,
		ANISOTROPIC,
		COMPARISON_MIN_MAG_MIP_POINT,
		COMPARISON_MIN_MAG_POINT_MIP_LINEAR,
		COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT,
		COMPARISON_MIN_POINT_MAG_MIP_LINEAR,
		COMPARISON_MIN_LINEAR_MAG_MIP_POINT,
		COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		COMPARISON_MIN_MAG_LINEAR_MIP_POINT,
		COMPARISON_MIN_MAG_MIP_LINEAR,
		COMPARISON_ANISOTROPIC,
		MINIMUM_MIN_MAG_MIP_POINT,
		MINIMUM_MIN_MAG_POINT_MIP_LINEAR,
		MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		MINIMUM_MIN_POINT_MAG_MIP_LINEAR,
		MINIMUM_MIN_LINEAR_MAG_MIP_POINT,
		MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MINIMUM_MIN_MAG_LINEAR_MIP_POINT,
		MINIMUM_MIN_MAG_MIP_LINEAR,
		MINIMUM_ANISOTROPIC,
		MAXIMUM_MIN_MAG_MIP_POINT,
		MAXIMUM_MIN_MAG_POINT_MIP_LINEAR,
		MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT,
		MAXIMUM_MIN_POINT_MAG_MIP_LINEAR,
		MAXIMUM_MIN_LINEAR_MAG_MIP_POINT,
		MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR,
		MAXIMUM_MIN_MAG_LINEAR_MIP_POINT,
		MAXIMUM_MIN_MAG_MIP_LINEAR,
		MAXIMUM_ANISOTROPIC,
	};

	struct SamplerDesc
	{
		Filter filter = Filter::MIN_MAG_MIP_POINT;
		TextureAddressMode address_u = TextureAddressMode::CLAMP;
		TextureAddressMode address_v = TextureAddressMode::CLAMP;
		TextureAddressMode address_w = TextureAddressMode::CLAMP;
		float mip_lod_bias = 0;					//MIP 贴图层级的偏移
		uint32_t max_anisotropy = 0;			//控制 各向异性过滤（Anisotropic Filtering, AF），提高斜视角下的纹理质量。
		ComparisonFunc comparison_func = ComparisonFunc::NEVER;
		SamplerBorderColor border_color = SamplerBorderColor::TRANSPARENT_BLACK;
		float min_lod = 0;
		float max_lod = std::numeric_limits<float>::max();
	};

	class Sampler :public GraphicsDeviceChild
	{
	public:
		~Sampler();
		std::shared_ptr<void> internal_state;
		SamplerDesc desc;
		inline bool isValid() const{ return internal_state != nullptr; }
	};

	
};

template<>
struct enable_bitmask_operators<qyhs::graphics::GraphicsDeviceCapability>
{
	static constexpr bool enable = true;
};

template<>
struct enable_bitmask_operators<qyhs::graphics::BindFlag> {
	static constexpr bool enable = true;
};

template<>
struct enable_bitmask_operators<qyhs::graphics::ResourceState> {
	static constexpr bool enable = true;
};

template<>
struct enable_bitmask_operators<qyhs::graphics::ColorWrite> {
	static constexpr bool enable = true;
};

template<>
struct enable_bitmask_operators<qyhs::graphics::ResourceMiscFlag> {
	static constexpr bool enable = true;
};
template<>
struct enable_bitmask_operators<qyhs::graphics::RenderPassFlags> {
	static constexpr bool enable = true;
};