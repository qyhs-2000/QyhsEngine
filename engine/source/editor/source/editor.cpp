#include "include/editor.h"
#include "editor_input_manager.h"
namespace QYHS
{
	void QyhsEditor::initialize(QyhsEngine* engine)
	{
		assert(engine);
		m_engine = engine;
		g_editor_global_context.initialize();
	}

	void QyhsEditor::run()
	{
		float delta_time;
		while (true)
		{
			delta_time = m_engine->caculateDeltaTime();
			g_editor_global_context.m_input_manager->tick(delta_time);
			if (!m_engine->tick(delta_time))
			{
				return;
			}
		}
	}

}
