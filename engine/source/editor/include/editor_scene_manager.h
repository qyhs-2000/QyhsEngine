#pragma once
#include <memory>

#include "editor_axis.h"
#include "function/framework/game_object/game_object_id_allocator.h"
namespace qyhs
{
	class RenderEntity;
	class RenderCamera;
	class RenderSystem;
	class EditorSceneManager
	{
	public:
		enum class EditorAxisType
		{
			Translate = 0,
			Rotate,
			Scale
		};

		std::shared_ptr<RenderCamera> getEditorCamera(){ return m_camera; }
		uint32_t getMeshIDByPickedUV(Vector2 picked_id);
		void initialize();
		void tick(float delta_time);
		void GObjectSelected(size_t go_id);
		void drawSelectedEntityAxis();
		std::weak_ptr<class GameObject> getSelectedGameObject();
		RenderEntity* getAxisMeshByType(EditorAxisType axis_type);
		void uploadAxisResource();
		size_t updateCursorOnAxis(Vector2 cursor_uv,Vector2 window_size);
		inline EditorAxisType getEditorAxisMode() { return m_axis_mode; }
		inline void setEditorAxisMode(EditorAxisType axis_type) { m_axis_mode = axis_type; }
		EditorTranslateAxis  m_translate_axis_render_entity;
		EditorRotateAxis  m_rotate_axis_render_entity;
		EditorScaleAxis  m_scale_axis_render_entity;
		//float intersectPlaneRay(Vector3 normal, float d, Vector3 origin, Vector3 dir);
		void moveObject(const float &new_mouse_x,const float &new_mouse_y,const float & old_mouse_x,const float & old_mouse_y,const Vector2 & engine_window_size);
		GameObjectID m_selected_game_object_id{ k_invalid_gobject_id };
	private:
		std::shared_ptr<RenderCamera>    m_camera;
		std::shared_ptr<RenderSystem>    m_render_system;
		Matrix4x4 m_selected_gobject_matrix{ Matrix4x4::IDENTITY };
		EditorAxisType m_axis_mode{ EditorAxisType::Translate };
		size_t m_selected_axis{ 3 };
		bool m_is_show_axis = false;
	};
}