#pragma once

namespace QYHS
{
	struct EditorGlobalContextInitInfo
	{
		class WindowSystem* window_system;
	};

	class EditorGlobalContext
	{
	public:
		void initialize(EditorGlobalContextInitInfo & init_info);
		class WindowSystem* m_window_system{ nullptr };
		class EditorSceneManager* m_scene_manager{ nullptr };
		class EditorInputManager* m_input_manager{ nullptr };
	};

	extern EditorGlobalContext g_editor_global_context;
}