#pragma once
#include "engine.h"
#include "editor_global_context.h"
#include <function/render/ui/editor_ui.h>
//#include "function/global/global_context.h"
namespace QYHS
{
	class QyhsEditor
	{
	public:
		void initialize(QyhsEngine * engine);
		void run();
	private:
		QyhsEngine* m_engine;
		std::shared_ptr<EditorUI>  m_editor_ui;
	};
}
