#pragma once

namespace qyhs
{
	struct EditorGlobalContextInitInfo
	{
		class WindowSystem* window_system;
		class RenderSystem * render_system;
	};

	class EditorGlobalContext
	{
	public:
		void initialize(EditorGlobalContextInitInfo & init_info);
		void clear();
		class WindowSystem* m_window_system{ nullptr };
		class EditorSceneManager* m_scene_manager{ nullptr };
		class EditorInputManager* m_input_manager{ nullptr };
		class RenderSystem * m_render_system{nullptr};
	};

	extern EditorGlobalContext g_editor_global_context;
}