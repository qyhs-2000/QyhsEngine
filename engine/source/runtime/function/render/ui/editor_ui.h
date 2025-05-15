#pragma once
#include "function/render/window_system.h"
#include "window_ui.h"
#include "editor/include/editor_scene_manager.h"
#include "core/meta/reflection/reflection.h"
namespace qyhs
{
	class EditorUI :public WindowUI
	{
	public:
		EditorUI();
		virtual void initialize(WindowUIInitInfo* info) override;
		void setUIColorStyle();
		void showEditorUI();
		void showEditorMenu(bool* p_open);
		void showGameInstancesUI(bool* p_bool_show);
		void showComponentDetails(bool* p_bool_show);
		virtual void preRender() override final;
		void createClassUI(Reflection::ReflectionInstance &object_instance);
		void createLeafNodeUI(Reflection::ReflectionInstance &object_instance);
		void showEditorGameUI(bool *p_show_game_engine_window);
		void drawAxisToggleButton(std::string name, bool button_checked, EditorSceneManager::EditorAxisType axis_type);
	private:
		bool m_editor_show_menu_ui{ true };
		bool m_editor_show_game_instances{ true };
		bool m_editor_show_component_details{ true };
		bool m_game_engine_window_open{ true };
		std::unordered_map<std::string, std::function<void(const std::string&, void*)>> m_editor_ui_creators;
	};

}
