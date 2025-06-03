#pragma once
#include <cstdint>
#include <memory>
#include "function/render/rhi/graphics.h"
#include "function/ui/image.h"

#include "function/platform/platform.h"
#include "core/common_include.h"
namespace qyhs
{
	class CommandList
	{
	public:
		void* internal_state{ nullptr };
		bool isValid() const { return internal_state != nullptr; }
	private:
	};

	struct SwapChainDesc
	{
		unsigned int width = 0;
		unsigned int height = 0;
		uint32_t buffer_count = 2;
		graphics::Format format{ graphics::Format::B8G8R8A8_UNORM };
		float clear_color[4] = { 0,1,1,1 };
		bool vsync = true;
	};

	class SwapChain
	{
	public:
		SwapChain() = default;
		SwapChain(SwapChainDesc desc)
			: desc(desc)
		{
		}
		std::shared_ptr<void> internal_state;
		SwapChainDesc desc;
		bool isValid() { return internal_state != nullptr; }
	private:
	};

	struct RenderPassInfo
	{
		Format rt_formats[8] = {};
		Format ds_format = Format::UNKNOWN;
		size_t rt_count{ 0 };
		size_t sample_count{ 1 };
		const uint64_t get_hash() const
		{
			union Hasher
			{
				struct
				{
					uint64_t rt_format_0 : 6;
					uint64_t rt_format_1 : 6;
					uint64_t rt_format_2 : 6;
					uint64_t rt_format_3 : 6;
					uint64_t rt_format_4 : 6;
					uint64_t rt_format_5 : 6;
					uint64_t rt_format_6 : 6;
					uint64_t rt_format_7 : 6;
					uint64_t ds_format : 6;
					uint64_t sample_count : 3;
				} bits;
				uint64_t value;
			} hasher = {};
			static_assert(sizeof(Hasher) == sizeof(uint64_t));
			hasher.bits.rt_format_0 = (uint64_t)rt_formats[0];
			hasher.bits.rt_format_1 = (uint64_t)rt_formats[1];
			hasher.bits.rt_format_2 = (uint64_t)rt_formats[2];
			hasher.bits.rt_format_3 = (uint64_t)rt_formats[3];
			hasher.bits.rt_format_4 = (uint64_t)rt_formats[4];
			hasher.bits.rt_format_5 = (uint64_t)rt_formats[5];
			hasher.bits.rt_format_6 = (uint64_t)rt_formats[6];
			hasher.bits.rt_format_7 = (uint64_t)rt_formats[7];
			hasher.bits.ds_format = (uint64_t)ds_format;
			hasher.bits.sample_count = (uint64_t)sample_count;
			return hasher.value;
		}

		static RenderPassInfo from(const SwapChainDesc & desc)
		{
			RenderPassInfo info;
			info.rt_count = 1;
			info.rt_formats[0] = desc.format;
			return info;
		}

		static RenderPassInfo from(const RenderPassImage* images,uint32_t image_count)
		{
			RenderPassInfo info;
			for (int i = 0; i < image_count; ++i)
			{
				const RenderPassImage& image = images[i];
				const TextureDesc& desc = image.texture->desc;
				switch (image.type)
				{
				case RenderPassImage::Type::RENDERTARGET:
				{
					info.rt_formats[info.rt_count++] = desc.format;
				}
				break;
				case RenderPassImage::Type::DEPTH_STENCIL:
				{
					info.ds_format = desc.format;
				}
				break;
				default:
					break;
				}
				info.sample_count = desc.sample_count;
			}
			return info;
		}
	};

	class Viewport
	{
	public:
		Viewport() = default;
		Viewport(float x, float y, float width, float height)
			: x(x), y(y), width(width), height(height)
		{
		}

		float x = 0.0f;
		float y = 0.0f;
		float top_left_x = 0.0f;
		float top_left_y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float min_depth = 0;
		float max_depth = 1;
	private:
	};

	class RHI
	{
	public:
		static constexpr uint32_t BUFFER_COUNT = 2;
		enum QueueType
		{
			QUEUE_GRAPHICS = 0,
			QUEUE_COMPUTE,
			QUEUE_TRANSFER,
			QUEUE_VIDEO_DECODE,
			QUEUE_COUNT
		};
	public:
		virtual ~RHI() = 0;
		virtual void initialize() = 0;
		virtual void initialize2() = 0;
		virtual void prepareContext() = 0;
		virtual bool createSwapChain(platform::WindowType window, SwapChain* swapChain, SwapChainDesc desc) = 0;
		virtual bool createSampler(const SamplerDesc* sampler_desc, Sampler* sampler) = 0;
		virtual CommandList beginCommandList(QueueType queue = QUEUE_GRAPHICS) = 0;
		virtual void bindViewports(const CommandList& cmd_list, uint32_t viewport_count, Viewport* viewport) = 0;
		virtual void beginRenderPass(SwapChain* swapChain, CommandList& cmd_list) = 0;
		virtual void beginRenderPass(const RenderPassImage *image, uint32_t size, CommandList cmd,RenderPassFlags flags = RenderPassFlags::NONE) = 0;
		virtual void endRenderPass(CommandList cmd_list) = 0;
		virtual void submitCommandLists() = 0;
		virtual void bindScissorRects(uint32_t scissor_count, const Rect* rect, CommandList cmd) = 0;
		bool checkCapability(GraphicsDeviceCapability capability) { return has_flag(capabilities, capability); }
		virtual void drawImage(const graphics::Texture* texture, const qyhs::image::Params& params, CommandList cmd) {};
		virtual int getDescriptorIndex(const GPUResource* resource, SubresourceType type, int subresource = -1)const = 0;
		virtual bool createTexture(const TextureDesc* desc, graphics::Texture* texture,SubresourceData * initial_data = nullptr) = 0;
		virtual bool createBuffer(const GPUBufferDesc * desc, GPUBuffer* buffer,const std::function<void(void*)>& init_buffer_callback = nullptr) = 0;
		virtual void drawInstanced(uint32_t vertexCount, uint32_t instanceCount, uint32_t startVertexLocation, uint32_t startInstanceLocation, CommandList cmd) = 0;
		virtual int createSubresource(Texture* texture, SubresourceType type, uint32_t firstSlice, uint32_t sliceCount, uint32_t firstMip, uint32_t mipCount, const Format* format_change = nullptr, const ImageAspect* aspect = nullptr, const Swizzle* swizzle = nullptr, float min_lod_clamp = 0) const = 0;
		virtual int createSubresource(GPUBuffer* buffer, SubresourceType type, uint64_t offset, uint64_t size = ~0ull,const Format * foramt_change = nullptr) = 0;
		virtual void drawIndexedInstanced(uint32_t index_count, uint32_t instance_count, uint32_t start_index_location, int32_t base_vertex_location, uint32_t start_instance_location, CommandList cmd) = 0;
		virtual int getDescriptorIndex(const Sampler* sampler) = 0;
		virtual void pushConstants(const void* data, uint32_t size, CommandList cmd, uint32_t offset = 0) = 0;
		virtual void beginEvent(const std::string& name, CommandList cmd) = 0;
		virtual void bindStencilRef(uint32_t stencil_ref, CommandList cmd) = 0;
		virtual ShaderFormat getShaderFormat()const = 0;
		virtual bool createShader(ShaderStage stage, const void* shader_code, size_t shader_size, Shader* shader) = 0;
		virtual bool createPipelineState(const PipelineStateDesc* desc, PipelineState* state, const RenderPassInfo* renderpass_info = nullptr) = 0;
		virtual void draw(uint32_t vertex_count, uint32_t start_draw_location, CommandList cmd) = 0;
		virtual void drawIndexed(uint32_t indexCount, uint32_t startIndexLocation, int32_t baseVertexLocation, CommandList cmd) = 0;
		virtual void endEvent(CommandList cmd) = 0;
		virtual void bindPipelineState(const PipelineState* state, CommandList cmd) = 0;
		virtual void setName(Shader* shader, const char* name) const {};
		virtual void setName(GPUResource* resource, const char* name) const = 0;
		virtual RenderPassInfo getRenderPassInfo(CommandList cmd) const = 0;
		virtual void updateBuffer(const GPUBuffer* buffer, const void* data, CommandList cmd, uint64_t size = ~0, uint64_t offset = 0);
		virtual void copyBuffer(const GPUBuffer* dst_buffer, uint64_t dst_offset, const GPUBuffer* src_buffer, uint64_t src_offset,uint64_t size,CommandList cmd) = 0;
		virtual void bindIndexBuffer(const GPUBuffer* indexBuffer, const IndexBufferFormat format, uint64_t offset, CommandList cmd) = 0;
		template<typename T>
		void bindDynamicConstantBuffer(const T& data, uint32_t slot, CommandList cmd)
		{
			GPUAllocation allocation = allocateGPU(sizeof(T), cmd);
			std::memcpy(allocation.data, &data, sizeof(T));
			bindConstantBuffer(&allocation.buffer, slot, cmd, allocation.offset);
		};
		virtual void bindConstantBuffer(const GPUBuffer* buffer, uint32_t slot, CommandList cmd, uint32_t offset = 0) = 0;
		struct GPUAllocation
		{
			GPUBuffer buffer;
			uint64_t offset;
			void* data;
			inline bool isValid() { return data != nullptr && buffer.isValid(); }
		};
		struct GPUBufferAllocator
		{
			GPUBuffer buffer;
			uint64_t offset = 0ull;
			uint64_t alignment = 0ull;

			void reset()
			{
				offset = 0ull;
			}
		};
		virtual uint64_t getMinOffsetAlignment(const GPUBufferDesc* desc) const = 0;
		virtual GPUBufferAllocator& getFrameAllocator(CommandList cmd) = 0;
		GPUAllocation allocateGPU(uint64_t size, CommandList& cmd);
		static constexpr uint32_t getBufferCount() { return BUFFER_COUNT; }
		constexpr uint64_t getBufferIndex() const { return getFrameCount() % getBufferCount(); }
		constexpr uint64_t getFrameCount()const { return frame_count; }
	protected:
		graphics::GraphicsDeviceCapability capabilities = graphics::GraphicsDeviceCapability::NONE;		//feature of graphics device
		uint64_t frame_count{ 0 };
	};

	inline RHI::~RHI() = default;
}

namespace qyhs::rhi
{
	//extern std::shared_ptr<RHI> rhi;
	RHI*& getRHI();

}