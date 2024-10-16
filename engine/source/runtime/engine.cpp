#include "engine.h"

//#include <function/render/model.h>

#include "core/meta/reflection/reflection_register.h"

#include "function/global/global_context.h"
#include <function/render/render_system.h>
#include<function/render/window_system.h>
#include <function/framework/world/world_manager.h>
namespace QYHS
{
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
		g_runtime_global_context.m_window->pollEvents();
		const bool should_close_window = g_runtime_global_context.m_window->shouldCloseWindow();
		return !should_close_window;
	}

	void QyhsEngine::logicTick(double delta_time)
	{
		g_runtime_global_context.m_world_manager->tick(delta_time);
	}

	void QyhsEngine::renderTick()
	{
		g_runtime_global_context.m_render_system->tick();
	}

	/*void QyhsEngine::loadAssets()
	{
		loadglTFFile("E://VS_Project//QyhsEngine//QyhsRuntime//resource//model//CesiumMan//glTF-Embedded//character_anim.gltf");
	}*/

	/*void QyhsEngine::loadglTFFile(const std::string& path)
	{
		tinygltf::Model glTFInput;
		tinygltf::TinyGLTF gltf_context;
		std::string error, warning;

		bool file_loaded = gltf_context.LoadASCIIFromFile(&glTFInput, &error, &warning, path);
		if (!file_loaded)
		{
			throw std::runtime_error("failed to load gltf file,make sure to load current file path!");
		}

		m_model->loadResourceFromFile(glTFInput);


	}*/

}