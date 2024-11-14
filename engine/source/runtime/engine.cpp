#include "engine.h"

//#include <function/render/model.h>

#include "core/meta/reflection/reflection_register.h"

#include "function/global/global_context.h"
#include <function/render/render_system.h>
#include<function/render/window_system.h>
#include <function/framework/world/world_manager.h>
namespace QYHS
{
	bool g_is_editor_mode = true;
	void QyhsEngine::startEngine(const std::string& config_file_path)
	{
		Reflection::TypeMetaRegister::Register();
		g_runtime_global_context.startSystem(config_file_path);
	}

	void QyhsEngine::initialize()
	{
		current_time = glfwGetTime();
		//m_model = new GLTFModel();
	}

	double QyhsEngine::caculateDeltaTime()
	{
		double time = glfwGetTime();
		double delta_time = time - current_time;
		current_time = time;
		return delta_time;
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