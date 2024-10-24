#pragma once
#include <memory>

#include "editor_axis.h"
#include "function/framework/game_object/game_object_id_allocator.h"
namespace QYHS
{
	class RenderEntity;
	class RenderCamera;
	class RenderSystem;
	class EditorSceneManager
	{
		enum class EditorAxisType
		{
			Translate,
			Rotate,
			Scale
		};
	public:
		std::shared_ptr<RenderCamera> getEditorCamera(){ return m_camera; }
		uint32_t getMeshIDByPickedUV(Vector2 picked_id);
		void initialize();
		void GObjectSelected(size_t go_id);
		void drawSelectedEntityAxis();
		std::weak_ptr<class GameObject> getSelectedGameObject();
		RenderEntity* getAxisMeshByType(EditorAxisType axis_type);
		void uploadAxisResource();
		
		EditorTranslateAxis  m_translate_axis_render_entity;
		EditorRotateAxis  m_rotate_axis_render_entity;
		EditorScaleAxis  m_scale_axis_render_entity;
	private:
		std::shared_ptr<RenderCamera>    m_camera;
		std::shared_ptr<RenderSystem>    m_render_system;
		GameObjectID m_selected_game_object_id{ k_invalid_gobject_id };
		Matrix4x4 m_selected_gobject_matrix{ Matrix4x4::IDENTITY };
		EditorAxisType m_axis_mode{ EditorAxisType::Translate };
	};
}