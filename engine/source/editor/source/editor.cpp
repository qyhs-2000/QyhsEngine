#include "include/editor.h"
#include "editor_input_manager.h"
#include "editor_scene_manager.h"
#include "function/global/global_context.h"
namespace QYHS
{
	void QyhsEditor::initialize()
	{
		QyhsEngine::initialize();
		
		EditorGlobalContextInitInfo init_info = { g_runtime_global_context.m_window_system.get(),g_runtime_global_context.m_render_system.get() };
		g_editor_global_context.initialize(init_info);

		g_editor_global_context.m_scene_manager->uploadAxisResource();
		if (show_ui)
		{
			m_editor_ui = std::make_shared<EditorUI>();
			WindowUIInitInfo ui_init_info = {g_runtime_global_context.m_window_system,
				g_runtime_global_context.m_render_system};
			m_editor_ui->initialize(&ui_init_info);

		}

		registerEditorTickComponent("TransformComponent");
		registerEditorTickComponent("MeshComponent");
	}

	void QyhsEditor::clear()
	{
		g_editor_global_context.clear();
	}

	void QyhsEditor::update(float delta_time)
	{
		g_editor_global_context.m_input_manager->tick(delta_time);
		g_editor_global_context.m_scene_manager->tick(delta_time);
	}

	void QyhsEditor::registerEditorTickComponent(const std::string& component_name)
	{
		g_editor_tick_component_types.insert(component_name);
	}

}
