
#pragma once
#include <vector>
#include <memory>
#include "function/render/graphics.h"
#include <Windows.h>
namespace QYHS
{
	static constexpr uint64_t buffer_count = 3;
	enum QueueType
	{
		QUEUE_GRAPHICS,
		QUEUE_COMPUTE,
		QUEUE_COUNT
	};

	struct CommandList
	{
		void* internal_command_list;
		bool isValid() { return internal_command_list != nullptr; }

	};

	//it is resource of rhi
	struct RHIChild
	{
		std::shared_ptr<void> internal_state;
		bool isValid() { return internal_state != nullptr; }
		virtual ~RHIChild() = default;
	};

	struct SwapChainDesc
	{
		int width{ 0 };
		int height{ 0 };
		bool allow_hdr{ true };
		uint32_t buffer_count{ buffer_count };
		Format format = Format::R10G10B10A2_UNORM;
		float clear_color[4] = { 0,0,0,1 };
	};

	class SwapChain:public RHIChild
	{
	public:
		SwapChainDesc desc;
	private:

	};

	class RHI
	{
	public:
		virtual ~RHI() = 0;
		virtual void initialize() = 0;
		virtual void prepareContext() = 0;
		virtual CommandList beginCommandList(QueueType queue = QUEUE_GRAPHICS) = 0;
		virtual void submitCommandLists() = 0;
		virtual void endRenderPass(CommandList commandlist) = 0;
		virtual bool createSwapChain(const SwapChainDesc * desc,SwapChain & swap_chain,HWND hwnd)const = 0;
		virtual void bindViewport(CommandList commandlist, uint32_t bind_count, Viewport* viewport) = 0;
		virtual void beginRenderPass(SwapChain * swap_chain,CommandList commandlist) = 0;
		uint64_t getFrameCount() { return frame_count; }
		uint64_t getBufferCount() { return buffer_count; }
		uint64_t getBufferIndex() { return getFrameCount() % getBufferCount(); }
		
	protected:
		uint64_t cmd_count{0};
		
		uint64_t frame_count;
	};

	inline RHI::~RHI() = default;
}