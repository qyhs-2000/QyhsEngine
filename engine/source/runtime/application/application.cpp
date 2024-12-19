#include "application.h"
#include "function/event/event_manager.h"
#include "function/render/render_path.h"
#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include <function/render/viewport.h>
namespace QYHS
{
	void Application::run()
	{
		if (!initialized)
		{
			initialize();
			initialized = true;
		}
		delta_time = float(timer.record_past_time());
		const float target_frame_delta_time = 1 / target_frame_rate;
		if (frame_rate_lock && delta_time < target_frame_delta_time)
		{
			timer::QuickSleep((target_frame_delta_time - delta_time) / 1000);
			delta_time += timer.record_past_time();
		}

		event_manager->fireEvent(EVENT_THREAD_SAFE_POINT);
		update(delta_time);
		render();

		CommandList cmd = m_rhi->beginCommandList();

		Viewport viewport;
		viewport.width = swap_chain.desc.width;
		viewport.height = swap_chain.desc.height;
		m_rhi->bindViewport(cmd,1, &viewport);
		
		m_rhi->beginRenderPass(&swap_chain, cmd);
		
		compose(cmd);
		m_rhi->endRenderPass(cmd);
		m_rhi->submitCommandLists();
	}



	void Application::render()
	{
		if (getActiveRenderPath() != nullptr)
		{
			getActiveRenderPath()->render();
		}
	}

	void Application::update(float delta_time)
	{
		auto active_renderer = getActiveRenderPath();
		if (active_renderer != nullptr)
		{
			active_renderer->update(delta_time);
			
		}
	}

	void Application::initialize()
	{
		event_manager = std::make_shared<EventManager>();

	}

	void Application::compose(CommandList cmd)
	{
		
	}

	void Application::setWindow(HWND hwnd)
	{
		m_hwnd = hwnd; 
		m_rhi = std::make_shared<VulkanRHI>();

		canvas.init(hwnd);
		SwapChainDesc desc;
		desc.width = canvas.getPhysicalWidth();
		desc.height = canvas.getPhysicalHeight();
		bool success = m_rhi->createSwapChain(&desc, swap_chain,m_hwnd);
	}
}