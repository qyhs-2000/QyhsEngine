#pragma once
#include <cstdint>
#include <memory>
#include "function/render/rhi/graphics.h"
#include "function/platform/platform.h"
namespace QYHS
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
		Format format{ Format::B8G8R8A8_UNORM };
		float clear_color[4] = { 0,0,255,1 };
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
			QUEUE_COUNT
		};
	public:
		virtual ~RHI() = 0;
		virtual void initialize() = 0;
		virtual void initialize2() = 0;
		virtual void prepareContext() = 0;
		virtual bool createSwapChain(platform::WindowType window,SwapChain* swapChain,SwapChainDesc desc) = 0;
		virtual CommandList beginCommandList(QueueType queue = QUEUE_GRAPHICS) = 0;
		virtual void bindViewports(CommandList& cmd_list, uint32_t viewport_count, Viewport* viewport) = 0;
		virtual void beginRenderPass(SwapChain* swapChain, CommandList& cmd_list) = 0;
		virtual void endRenderPass(CommandList& cmd_list) = 0;
		virtual void submitCommandLists(CommandList& cmd) = 0;
	protected:
	
	};

	inline RHI::~RHI() = default;
}