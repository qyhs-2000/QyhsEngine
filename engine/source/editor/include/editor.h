#pragma once
#include "engine.h"
#include "editor_global_context.h"
#include <function/render/ui/editor_ui.h>
//#include "function/global/global_context.h"
namespace QYHS
{
	extern std::unordered_set<std::string> g_editor_tick_component_types;
	class QyhsEditor
	{
	public:
		void initialize(QyhsEngine * engine);
		void run();
		void clear();
		void registerEditorTickComponent(const std::string &component_name);
	private:
		QyhsEngine* m_engine;
		std::shared_ptr<EditorUI>  m_editor_ui;
	};
}
