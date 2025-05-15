#include "editor_scene_manager.h"
#include <iostream>
#include <function/framework/game_object/game_object.h>
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/world/world_manager.h"
#include "function/global/global_context.h"
#include "core/math/matrix4.h"
#include "function/render/render_system.h"
#include <editor.h>

namespace qyhs
{
	void EditorSceneManager::initialize()
	{
		m_camera = g_runtime_global_context.m_render_system->getRenderCamera();
		m_render_system = g_runtime_global_context.m_render_system;
	}

	void EditorSceneManager::tick(float delta_time)
	{
		std::shared_ptr<GameObject> selected_game_object = getSelectedGameObject().lock();
		if (selected_game_object)
		{
			TransformComponent* transform_component = selected_game_object->TryGetComponent(TransformComponent);
			if (transform_component)
			{
				transform_component->setDirty(true);
			}
		}
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
			drawSelectedEntityAxis();
			m_is_show_axis = true;
		}
		else
		{
			m_is_show_axis = false;
		}
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
		RenderEntity* value;
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
		auto& instance_allocator = m_render_system->getInstanceIDAllocator();
		auto& mesh_allocator = m_render_system->getMeshAssetIDAllocator();
		GameObjectPartId translate_axis_id = { 0XFFAA,0XFFAA };
		m_translate_axis_render_entity.m_instance_id = instance_allocator.allocateGUId(translate_axis_id);

		MeshSourceDesc translate_axis_mesh = { "%%axis_translate%%" };
		m_translate_axis_render_entity.mesh_asset_id = mesh_allocator.allocateGUId(translate_axis_mesh);

		GameObjectPartId rotate_axis_id = { 0XFFBB,0XFFBB };
		m_rotate_axis_render_entity.m_instance_id = instance_allocator.allocateGUId(rotate_axis_id);

		MeshSourceDesc rotate_axis_mesh = { "%%axis_translate%%" };
		m_rotate_axis_render_entity.mesh_asset_id = mesh_allocator.allocateGUId(rotate_axis_mesh);

		GameObjectPartId scale_axis_id = { 0XFFCC,0XFFCC };
		m_scale_axis_render_entity.m_instance_id = instance_allocator.allocateGUId(scale_axis_id);

		MeshSourceDesc scale_axis_mesh = { "%%axis_translate%%" };
		m_scale_axis_render_entity.mesh_asset_id = mesh_allocator.allocateGUId(scale_axis_mesh);

		std::vector<RenderEntity* > entities = { &m_translate_axis_render_entity,&m_rotate_axis_render_entity,&m_scale_axis_render_entity };
		std::vector<RenderMeshData > mesh_data = { m_translate_axis_render_entity.m_mesh_data,m_rotate_axis_render_entity.m_mesh_data,m_scale_axis_render_entity.m_mesh_data };

		for (int i = 0; i < entities.size(); ++i)
		{
			m_render_system->uploadGameResource(entities[i], mesh_data[i]);
		}

	}

	float intersectPlaneRay(Vector3 normal, float d, Vector3 origin, Vector3 dir)
	{
		float deno = normal.dotProduct(dir);
		if (fabs(deno) < 0.0001)
		{
			deno = 0.0001;
		}

		return -(normal.dotProduct(origin) + d) / deno;
	}

	size_t EditorSceneManager::updateCursorOnAxis(Vector2 cursor_uv, Vector2 window_size)
	{
		float camera_fov = m_camera->getCameraFOVY();
		float camera_near_face_distance = (window_size.y / 2) / tan(Degree(camera_fov).valueRadians()/2);
		Vector3 camera_forward = m_camera->forward();
		Vector3 camera_right = m_camera->right();
		Vector3 camera_up = m_camera->up();
		Vector3 camera_position = m_camera->getCameraPos();

		if (m_selected_game_object_id == k_invalid_gobject_id)
		{
			return m_selected_axis;
		}

		Vector2 screen_uv = Vector2(cursor_uv.x, 1 - cursor_uv.y) - Vector2(0.5f, 0.5f);
		Vector3 world_ray_direction = camera_forward * camera_near_face_distance +
			camera_right * (float)screen_uv.x * window_size.x +
			camera_up * (float)screen_uv.y * window_size.y;

		RenderEntity* axis_entity = getAxisMeshByType(m_axis_mode);
		m_selected_axis = 3;
		if (m_is_show_axis == false)
		{
			return m_selected_axis;
		}
		Matrix4x4 model_matrix = axis_entity->model_matrix;
		Vector3 translation;
		Quaternion rotation;
		Vector3 scale;
		model_matrix.decomposition(translation, scale, rotation);

		Vector4 local_ray_origin = model_matrix.inverse() * Vector4(camera_position, 1.0f);
		Vector3 local_ray_origin_xyz = Vector3(local_ray_origin.x, local_ray_origin.y, local_ray_origin.z);
		Quaternion inverse_rotation = rotation.inverse();
		inverse_rotation.normalise();
		Vector3 local_ray_direction = inverse_rotation * world_ray_direction;


		Vector3 normal[3] = { Vector3(1.0,0.0,0.0),Vector3(0.0,1.0,0.0),Vector3(0.0,0.0,1.0) };
		float interact_dist[3] = {													//distance from ray origin to face yoz,xoz,xoy
			intersectPlaneRay(normal[0],0,  local_ray_origin_xyz,local_ray_direction),
			intersectPlaneRay(normal[1],0,  local_ray_origin_xyz,local_ray_direction),
			intersectPlaneRay(normal[2],0,  local_ray_origin_xyz,local_ray_direction)
		};

		Vector3 interact_pt[3] = {													//interact point position 
			local_ray_origin_xyz + local_ray_direction * interact_dist[0],
			local_ray_origin_xyz + local_ray_direction * interact_dist[1],
			local_ray_origin_xyz + local_ray_direction * interact_dist[2]
		};

		float max_dist = 0.0f;
		const float DIST_THRESHOLD = 2.6f;
		const float AXIS_MIN_EDGE = 0.1f;
		const float AXIS_MAX_EDGE = 2.0f;
		const float AXIS_LENGTH = 2.0f;
		for (int i = 0; i < 3; ++i)
		{
			float alpha = Math::abs((normal[i].dotProduct(local_ray_direction)));
			if (alpha <= 0.1)										//when ray is almost parallel with face
			{
				int index00 = (i + 1) % 3;
				int index01 = 3 - i - index00;
				int index10 = (i + 2) % 3;
				int index11 = 3 - i - index10;
				int axis_dist = (Math::abs(interact_pt[index00][i]) + Math::abs(interact_pt[index10][i])) / 2;
				if (axis_dist >= DIST_THRESHOLD)				//if it is too far from parallel face
				{
					continue;
				}

				if ((interact_pt[index00][index01] > AXIS_MIN_EDGE) && (interact_pt[index00][index01] < AXIS_LENGTH) && (interact_pt[index00][index01] > max_dist) &&
					(Math::abs(interact_pt[00][i] < AXIS_MAX_EDGE)))
				{
					max_dist = interact_pt[index00][index01];
					m_selected_axis = index01;
				}
				if ((interact_pt[index10][index11] > AXIS_MIN_EDGE) && (interact_pt[index10][index11] < AXIS_LENGTH) && (interact_pt[index10][index11] > max_dist) &&
					(Math::abs(interact_pt[index10][i] < AXIS_MAX_EDGE)))
				{
					max_dist = interact_pt[index10][index11];
					m_selected_axis = index11;
				}
			}
		}
		if (m_selected_axis == 3)
		{
			float min_dist = 1e10f;
			for (int i = 0; i < 3; ++i)
			{
				int index_alpha = (i + 1) % 3;
				int index_beta = (i + 2) % 3;
				float dist = Math::sqr(interact_pt[index_alpha][index_beta]) + Math::sqr(interact_pt[index_beta][index_alpha]);
				if ((interact_pt[index_alpha][i] > AXIS_MIN_EDGE) && (interact_pt[index_alpha][i] < AXIS_MAX_EDGE) && (dist < DIST_THRESHOLD) &&
					(dist < min_dist))
				{
					min_dist = dist;
					m_selected_axis = i;
					std::cout << "selected axis changed to " << m_selected_axis << std::endl;
				}
			}
		}
		m_render_system->setSelectedAxis(m_selected_axis);
		return m_selected_axis;
	}

	void EditorSceneManager::moveObject(const float &new_mouse_x,const float &new_mouse_y,const float & old_mouse_x,const float & old_mouse_y,const Vector2 & engine_window_size)
	{
		if (((new_mouse_x == old_mouse_x) && (new_mouse_y == old_mouse_y))||(m_selected_axis == 3))
		{
			return;
		}
		Vector3 gobject_translation;
		Quaternion rotation;
		Vector3 scale;
		Matrix4x4 gobject_model_matrix = m_selected_gobject_matrix;
		gobject_model_matrix.decomposition(gobject_translation,scale, rotation );
		Matrix4x4 model_translate_matrix;
		model_translate_matrix.makeTrans(gobject_translation);
		Vector2 delta_mouse_uv = Vector2(new_mouse_x - old_mouse_x, new_mouse_y - old_mouse_y);
		float angularVelocity = 18.f / std::max(engine_window_size.x, engine_window_size.y);

		std::shared_ptr<GameObject> selected_game_object = getSelectedGameObject().lock();
		TransformComponent* transform_component = selected_game_object->TryGetComponent(TransformComponent);
		Matrix4x4 model_matrix = transform_component->getMatrix();
		Matrix4x4 view_matrix = m_camera->getViewMatrix();
		Matrix4x4 projection_matrix = m_camera->getProjMatrix();
		Vector4 model_origin_clip_position = projection_matrix * view_matrix * Vector4(gobject_translation,1.0f);
		model_origin_clip_position /= model_origin_clip_position.w;
		Vector2 model_origin_clip_uv = Vector2((model_origin_clip_position.x + 1) / 2, (model_origin_clip_position.y + 1) / 2);     //make uv range from 0 to 1;
		
		Vector4 axis_x_direction = { 1,0,0 ,1};
		Vector4 axis_x_clip_direction =  projection_matrix * view_matrix * model_translate_matrix*axis_x_direction;
		axis_x_clip_direction /= axis_x_clip_direction.w;
		Vector2 axis_x_clip_uv = Vector2((axis_x_clip_direction.x + 1) / 2, (axis_x_clip_direction.y + 1) / 2);
		Vector2 axis_x_direction_uv = axis_x_clip_uv - model_origin_clip_uv;
		axis_x_direction_uv.normalise();

		Vector4 axis_y_direction = { 0,1,0 ,1};
		Vector4 axis_y_clip_direction =  projection_matrix * view_matrix * model_translate_matrix*axis_y_direction;
		axis_y_clip_direction /= axis_y_clip_direction.w;
		Vector2 axis_y_clip_uv = Vector2((axis_y_clip_direction.x + 1) / 2, (axis_y_clip_direction.y + 1) / 2);
		Vector2 axis_y_direction_uv = axis_y_clip_uv - model_origin_clip_uv;
		axis_y_direction_uv.normalise();

		Vector4 axis_z_direction = { 0,0,1 ,1};
		Vector4 axis_z_clip_direction =  projection_matrix * view_matrix * model_translate_matrix*axis_z_direction;
		axis_z_clip_direction /= axis_z_clip_direction.w;
		Vector2 axis_z_clip_uv = Vector2((axis_z_clip_direction.x + 1) / 2, (axis_z_clip_direction.y + 1) / 2);
		Vector2 axis_z_direction_uv = axis_z_clip_uv - model_origin_clip_uv;
		axis_z_direction_uv.normalise();

		RenderEntity* axis_entitiy = &m_translate_axis_render_entity;
		Vector3 move_vector{ 0,0,0 };
		if (m_selected_axis == 0)
		{
			move_vector.x = delta_mouse_uv.dotProduct(axis_x_direction_uv) * angularVelocity;
		}
		else if (m_selected_axis == 1)
		{
			move_vector.y = delta_mouse_uv.dotProduct(axis_y_direction_uv) * angularVelocity;
		}
		else if (m_selected_axis == 2)
		{
			move_vector.z = delta_mouse_uv.dotProduct(axis_z_direction_uv) * angularVelocity;
		}
		Matrix4x4 translate_matrix;
		translate_matrix.makeTrans(move_vector);
		Matrix4x4 new_model_matrix = model_translate_matrix * translate_matrix;
		Vector3 new_translation;
		Vector3 new_scale;
		Quaternion new_rotation;
		new_model_matrix.decomposition(new_translation, new_scale, new_rotation);
		Matrix4x4 axis_model_matrix = Matrix4x4::getTrans(new_translation) * Matrix4x4::buildScaleMatrix(1.f, 1.f, 1.f);
		
		m_translate_axis_render_entity.model_matrix = axis_model_matrix;
		g_editor_global_context.m_render_system->setVisibleAxis(m_translate_axis_render_entity);
		
		transform_component->setPosition(new_translation);
		m_selected_gobject_matrix = new_model_matrix;

	}

	
}
