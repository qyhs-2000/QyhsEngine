#include "engine.h"

#include "core/meta/reflection/reflection_register.h"

#include "function/global/global_context.h"
#include <function/render/render_system.h>
#include<function/render/window_system.h>
#include <function/framework/world/world_manager.h>
#include "core/initializer.h"
#include <imgui/imgui.h>
#include "function/ui/font.h"
#include "function/input/input.h"
#include "core/event/event_handler.h"
namespace qyhs
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
		auto &rhi = qyhs::rhi::getRHI();
		rhi = m_rhi.get();
		initializer::initializeComponentAsync();
		//m_rhi = g_runtime_global_context.m_render_system->getRHI();
		//auto rhi = qyhs::rhi::getRHI();
		//rhi = m_rhi.get();
	}

	void QyhsEngine::initialize2()
	{
		if (initialized)
		{
			return;
		}
		initialized = true;
		auto &rhi = qyhs::rhi::getRHI();
		rhi = m_rhi.get();
		qyhs::initializer::initializeComponentAsync();
	}

	void QyhsEngine::run()
	{
		createWindow();
		float delta_time;

		//engine loop
		while (!shouldWindowClose())
		{
			if (!initialized)
			{
				initialize();
			}

			if (!initializer::initializeFinished())
			{
				continue;
			}
			delta_time = caculateDeltaTime();
			update(delta_time);
			if (!tick(delta_time))
			{
				return;
			}
		}
	}

	void QyhsEngine::activatePath(RenderPath* render_path)
	{
		if (render_path != nullptr)
		{
			render_path->init(canvas);
			if (active_path != nullptr)
			{
				active_path->deactivate();
			}
			active_path = render_path;
			render_path->start();
		}
	}

	void QyhsEngine::run2()
	{
		
		if (!initialized)
		{
			initialize2();
			initialized = true;
		}

		qyhs::font::updateAtlas(canvas.getDPIScaling());

		float delta_time = float(timer.record_elapsed_time());
		const float target_frame_time = 1.0f / target_frame_rate;
		if (frame_rate_lock && delta_time < target_frame_time)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds((int)((target_frame_time - delta_time) * 1000)));
			delta_time = float(timer.record_elapsed_time());
		}

		if (!initializer::initializeFinished())
		{
			return;
		}

		input::Update(window, canvas);
		eventhandler::fireEvent(eventhandler::EVENT_THREAD_SAFE_POINT, 0);
		update(delta_time);
		render();

		CommandList cmd_list = m_rhi->beginCommandList();
		Viewport viewport;
		viewport.width = (float)swapchain.desc.width;
		viewport.height = (float)swapchain.desc.height;

		m_rhi->bindViewports(cmd_list,1, &viewport);
		m_rhi->beginRenderPass(&swapchain, cmd_list);
		Rect scissorRect;
		scissorRect.bottom = (int32_t)(canvas.getPhysicalHeight());
		scissorRect.left = (int32_t)(0);
		scissorRect.right = (int32_t)(canvas.getPhysicalWidth());
		scissorRect.top = (int32_t)(0);

		m_rhi->bindScissorRects(1, &scissorRect, cmd_list);
		compose(cmd_list);
		m_rhi->endRenderPass(cmd_list);
		m_rhi->submitCommandLists();


		input::ClearForNextFrame();
	}

	void QyhsEngine::render()
	{
		auto active_path = getActivePath();
		if (active_path != nullptr)
		{
			active_path->render();
		}
		qyhs::image::Params params;
		//image::draw(nullptr,params, );
	}

	void QyhsEngine::compose(CommandList& cmd_list)
	{
		if (getActivePath() != nullptr)
		{
			getActivePath()->compose(cmd_list);
		}

		showInformation();
	}

	void QyhsEngine::showInformation()
	{
		//std::string information = "FPS: " + std::to_string(1.0f / timer.record_elapsed_time());

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
			desc.buffer_count = 3;
			desc.format = Format::R10G10B10A2_UNORM;
		}
		desc.width = canvas.getPhysicalWidth();
		desc.height = canvas.getPhysicalHeight();
		bool success = m_rhi->createSwapChain(window,&swapchain, desc);
		assert(success);
	}

	void QyhsEngine::update(float delta_time)
	{
		if (getActivePath() != nullptr)
		{
			getActivePath()->update(delta_time);
		}
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