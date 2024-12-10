#pragma once
#include "engine.h"
#include "editor_global_context.h"
#include <function/render/ui/editor_ui.h>
#include "application/application.h"
//#include "function/global/global_context.h"
namespace QYHS
{
	extern std::unordered_set<std::string> g_editor_tick_component_types;
	class QyhsEditor:public QyhsEngine
	{
	public:
		void initialize() override;
		void clear();
		virtual void update(float delta_time) override;
		void registerEditorTickComponent(const std::string &component_name);
		
	private:
		std::shared_ptr<EditorUI>  m_editor_ui;
	};
}
