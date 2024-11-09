#include "include/editor.h"
#include "editor_input_manager.h"
#include "editor_scene_manager.h"
#include "function/global/global_context.h"
namespace QYHS
{
	void QyhsEditor::initialize(QyhsEngine* engine)
	{
		assert(engine);
		m_engine = engine;
		EditorGlobalContextInitInfo init_info = { g_runtime_global_context.m_window_system.get(),g_runtime_global_context.m_render_system.get() };
		g_editor_global_context.initialize(init_info);

		g_editor_global_context.m_scene_manager->uploadAxisResource();

		m_editor_ui = std::make_shared<EditorUI>();
		WindowUIInitInfo ui_init_info = {g_runtime_global_context.m_window_system,
			g_runtime_global_context.m_render_system};
		m_editor_ui->initialize(&ui_init_info);
	}

	void QyhsEditor::run()
	{
		float delta_time;
		while (true)
		{
			delta_time = m_engine->caculateDeltaTime();
			g_editor_global_context.m_input_manager->tick(delta_time);
			g_editor_global_context.m_scene_manager->tick(delta_time);
			if (!m_engine->tick(delta_time))
			{
				return;
			}
		}
	}

}
