#include "editor_scene_manager.h"
#include <iostream>
#include <function/framework/game_object/game_object.h>
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/world/world_manager.h"
#include "function/global/global_context.h"
#include "core/math/matrix4.h"
#include "function/render/render_system.h"
#include <editor.h>

namespace QYHS
{
	void EditorSceneManager::initialize()
	{
		m_camera = g_runtime_global_context.m_render_system->getRenderCamera();
		m_render_system = g_runtime_global_context.m_render_system;
	}

	uint32_t EditorSceneManager::getMeshIDByPickedUV(Vector2 picked_id)
	{
		return m_render_system->getMeshIDByPickedUV(picked_id);
	}

	void EditorSceneManager::GObjectSelected(size_t go_id)
	{
		if (m_selected_game_object_id == go_id)
		{
			return;
		}
		m_selected_game_object_id = go_id;

		std::shared_ptr<GameObject> selected_game_object = getSelectedGameObject().lock();
		if (selected_game_object)
		{
			TransformComponent* transform_comp = selected_game_object->TryGetComponent(TransformComponent);
			m_selected_gobject_matrix = transform_comp->getMatrix();
		}

		drawSelectedEntityAxis();
		std::cout << go_id << std::endl;
	}

	std::weak_ptr<GameObject> EditorSceneManager::getSelectedGameObject()
	{
		std::weak_ptr<GameObject> selected_game_object;
		if (m_selected_game_object_id != k_invalid_gobject_id)
		{
			std::shared_ptr<Level> level = g_runtime_global_context.m_world_manager->getCurrentActiveLevel().lock();
			if (level)
			{
				selected_game_object = level->getGameObjectByID(m_selected_game_object_id);
			}
		}
		return selected_game_object;
	}
	void EditorSceneManager::drawSelectedEntityAxis()
	{
		std::shared_ptr<GameObject> selected_game_object = getSelectedGameObject().lock();
		if (selected_game_object != nullptr)
		{
			Vector3 scale;
			Quaternion rotation;
			Vector3 translation;
			TransformComponent* selected_transform_comp = selected_game_object->TryGetComponent(TransformComponent);
			selected_transform_comp->getMatrix().decomposition(translation, scale, rotation);
			Matrix4x4 translation_matrix = Matrix4x4::getTrans(translation);
			Matrix4x4 scale_matrix = Matrix4x4::buildScaleMatrix(1.0f, 1.0f, 1.0f);
			Matrix4x4 axis_model_matrix = translation_matrix * scale_matrix;
			RenderEntity* selected_axis = getAxisMeshByType(m_axis_mode);

			if (m_axis_mode == EditorAxisType::Translate || m_axis_mode == EditorAxisType::Scale)
			{
				selected_axis->model_matrix = axis_model_matrix;
			}
			else
			{
				selected_axis->model_matrix = axis_model_matrix * Matrix4x4(rotation);
			}

			g_editor_global_context.m_render_system->setVisibleAxis(*selected_axis);
		}
		else
		{
			g_editor_global_context.m_render_system->setVisibleAxis(std::nullopt);
		}


	}

	RenderEntity* EditorSceneManager::getAxisMeshByType(EditorAxisType axis_type)
	{
		RenderEntity * value;
		switch (axis_type)
		{
		case EditorAxisType::Translate:
			value = &m_translate_axis_render_entity;
		case EditorAxisType::Rotate:
			value = &m_rotate_axis_render_entity;
		case EditorAxisType::Scale:
			value = &m_scale_axis_render_entity;
		default:
			break;
		}
		return value;
	}

	void EditorSceneManager::uploadAxisResource()
	{
		auto & instance_allocator = m_render_system->getInstanceIDAllocator();
		auto & mesh_allocator = m_render_system->getMeshAssetIDAllocator();
		GameObjectPartId translate_axis_id = {0XFFAA,0XFFAA};
		m_translate_axis_render_entity.m_instance_id = instance_allocator.allocateGUId(translate_axis_id);

		MeshSourceDesc translate_axis_mesh = {"%%axis_translate%%"};
		m_translate_axis_render_entity.mesh_asset_id = mesh_allocator.allocateGUId(translate_axis_mesh);

		GameObjectPartId rotate_axis_id = {0XFFBB,0XFFBB};
		m_rotate_axis_render_entity.m_instance_id = instance_allocator.allocateGUId(rotate_axis_id);

		MeshSourceDesc rotate_axis_mesh = {"%%axis_translate%%"};
		m_rotate_axis_render_entity.mesh_asset_id = mesh_allocator.allocateGUId(rotate_axis_mesh);

		GameObjectPartId scale_axis_id = {0XFFCC,0XFFCC};
		m_scale_axis_render_entity.m_instance_id = instance_allocator.allocateGUId(scale_axis_id);

		MeshSourceDesc scale_axis_mesh = {"%%axis_translate%%"};
		m_scale_axis_render_entity.mesh_asset_id =  mesh_allocator.allocateGUId(scale_axis_mesh);

		std::vector<RenderEntity * > entities = {&m_translate_axis_render_entity,&m_rotate_axis_render_entity,&m_scale_axis_render_entity};
		std::vector<RenderMeshData > mesh_data = {m_translate_axis_render_entity.m_mesh_data,m_rotate_axis_render_entity.m_mesh_data,m_scale_axis_render_entity.m_mesh_data};

		for(int i = 0;i<entities.size();++i)
		{
			m_render_system->uploadGameResource(entities[i],mesh_data[i]);
		}
		
	}

}
