#include "editor_global_context.h"

#include "editor_input_manager.h"
#include "editor_scene_manager.h"

#include "function/render/window_system.h"
namespace QYHS
{
	EditorGlobalContext g_editor_global_context;

	void EditorGlobalContext::initialize()
	{
		m_input_manager = new EditorInputManager();
		m_scene_manager = new EditorSceneManager();

		m_scene_manager->initialize();
		m_input_manager->initialize();

		m_window_system = new WindowSystem();
	}
}