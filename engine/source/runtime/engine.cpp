#include "engine.h"

#include "core/meta/reflection/reflection_register.h"

#include "function/global/global_context.h"
#include <function/render/render_system.h>
#include<function/render/window_system.h>
#include <function/framework/world/world_manager.h>


namespace QYHS
{
	bool g_is_editor_mode = true;
	std::unordered_set<std::string> g_editor_tick_component_types{};
	void QyhsEngine::startEngine(const std::string& config_file_path)
	{
		Reflection::TypeMetaRegister::Register();
		g_runtime_global_context.startSystem(config_file_path);
	}

	void QyhsEngine::createWindow()
	{
		g_runtime_global_context.createWindow();
	}

	bool QyhsEngine::shouldWindowClose()
	{
		return g_runtime_global_context.m_window_system->shouldCloseWindow();
	}

	void QyhsEngine::initialize()
	{
		if (initialized)
		{
			return;
		}
		initialized = true;
		startEngine(engine_config_file);
		current_time = glfwGetTime();
		m_rhi = g_runtime_global_context.m_render_system->getRHI();
	}

	void QyhsEngine::initialize2()
	{
		if (initialized)
		{
			return;
		}
		initialized = true;
		
	}

	void QyhsEngine::run()
	{
		createWindow();
		float delta_time;

		while (!shouldWindowClose())
		{
			if (!initialized)
			{
				initialize();
				initialized = true;
			}
			delta_time = caculateDeltaTime();
			update(delta_time);
			if (!tick(delta_time))
			{
				return;
			}
		}
	}

	void QyhsEngine::run2()
	{
		
		if (!initialized)
		{
			initialize2();
			initialized = true;
		}
		float delta_time = float(timer.record_elapsed_time());
		const float target_frame_time = 1.0f / target_frame_rate;
		if (frame_rate_lock && delta_time < target_frame_time)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)((target_frame_time - delta_time) * 1000)));
			delta_time = float(timer.record_elapsed_time());
		}

		CommandList cmd_list = m_rhi->beginCommandList();
		Viewport viewport;
		viewport.width = (float)swapchain.desc.width;
		viewport.height = (float)swapchain.desc.height;

		m_rhi->bindViewports(cmd_list,1, &viewport);
		m_rhi->beginRenderPass(&swapchain, cmd_list);
		//compose(cmd);
		m_rhi->endRenderPass(cmd_list);
		m_rhi->submitCommandLists(cmd_list);

	}

	void QyhsEngine::setWindow(platform::WindowType window)
	{
		this->window = window;
		
		canvas.init(window);
		SwapChainDesc desc;
		if (swapchain.isValid())
		{
			desc = swapchain.desc;
		}
		else
		{
			desc.format = Format::R10G10B10A2_UNORM;
		}
		bool success = m_rhi->createSwapChain(window,&swapchain, desc);
		assert(success);
	}

	void QyhsEngine::update(float delta_time)
	{

	}

	double QyhsEngine::caculateDeltaTime()
	{
		double time = glfwGetTime();
		double delta_time = time - current_time;
		current_time = time;
		return delta_time;
	}

	QyhsEngine::QyhsEngine()
	{

		
	}

	bool QyhsEngine::tick(double delta_time)
	{
		logicTick(delta_time);
		g_runtime_global_context.m_render_system->swapLogicRenderData();
		renderTick();
		g_runtime_global_context.m_window_system->pollEvents();
		const bool should_close_window = g_runtime_global_context.m_window_system->shouldCloseWindow();
		return !should_close_window;
	}

	void QyhsEngine::logicTick(double delta_time)
	{
		g_runtime_global_context.m_world_manager->tick(delta_time);
		g_runtime_global_context.m_input_system->tick(delta_time);
	}

	void QyhsEngine::renderTick()
	{
		g_runtime_global_context.m_render_system->tick();
	}

}