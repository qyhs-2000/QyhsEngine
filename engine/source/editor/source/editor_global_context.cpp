#include "editor_global_context.h"

#include "editor_input_manager.h"
#include "editor_scene_manager.h"

#include "function/render/window_system.h"
namespace qyhs
{
	EditorGlobalContext g_editor_global_context;

	void EditorGlobalContext::initialize(EditorGlobalContextInitInfo & info)
	{
		m_input_manager = new EditorInputManager();
		m_scene_manager = new EditorSceneManager();

		m_scene_manager->initialize();
		m_input_manager->initialize();

		m_window_system = info.window_system;
		m_render_system = info.render_system;
	}
	
	void EditorGlobalContext::clear()
	{
		delete m_input_manager;
		delete m_scene_manager;
	}
}