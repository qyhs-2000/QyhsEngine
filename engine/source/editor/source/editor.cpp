#include "include/editor.h"
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
			if (!m_engine->tick(delta_time))
			{
				return;
			}
		}
	}

}
