#pragma once
#include "window_ui.h"
#include "function/render/window_system.h"
namespace QYHS
{
	class EditorUI :public WindowUI
	{
	public:
		void showEditorMenu(bool* p_open);
		void showEditorUI();
		virtual void initialize(WindowUIInitInfo* info) override;
		void setUIColorStyle();
		virtual void preRender() override final;
	private:
		bool m_editor_show_menu_ui = true;
	};

}
