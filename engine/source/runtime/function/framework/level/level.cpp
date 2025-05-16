#include "level.h"
#include <assert.h>
#include <iostream>
#include "resource/type/level_resource.h"
#include "resource/type/world_resource.h"
#include "resource/common/object.h"
#include <resource/asset_manager/asset_manager.h>
#include "engine.h"
#include "core/utils/utils.h"
#include <core/utils/model_importer_gltf.h>


namespace qyhs
{
	std::shared_ptr<GameObject> Level::createGameObject(ObjectInstanceResource * instance_res)
	{
		GameObjectID game_object_id = m_go_id_allocator.allocateId();
		std::shared_ptr<GameObject> game_object;
		try
		{
			game_object = std::make_shared<GameObject>(game_object_id);
		}
		catch (const std::exception& e)
		{
			std::cout << "can't create game object!" << std::endl;
		}
		game_object->load(instance_res);
		m_game_objects.emplace(game_object_id, game_object);
		return game_object;
	}

	void Level::tick(double delta_time)
	{
		for (const auto& pair : m_game_objects)
		{
			assert(pair.second);
			pair.second->tick(delta_time);
		}
		if (m_current_active_character && !g_is_editor_mode)
		{
			m_current_active_character->tick(delta_time);
		}
	}

	bool Level::load(const std::string& level_url)
	{
		if (level_url.empty())return false;
		m_current_level_url = level_url;
		LevelResource level_res;
		const bool load_level_res_success = g_runtime_global_context.m_asset_manager->loadAsset(m_current_level_url, level_res);
		if (!load_level_res_success)
		{
			return false;
		}

		for (auto& instance : level_res.m_objects)
		{
			createGameObject(&instance);
		}

		for (auto& object_pair : m_game_objects)
		{
			std::shared_ptr<GameObject> object = object_pair.second;
			if (!object)
			{
				continue;
			}
			if (level_res.m_character_name == object->m_name)
			{
				m_current_active_character = std::make_shared<Character>(object);
				break;
			}
		}

		return true;
	}
	
	std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>& Level::getObjectsInLevel()
	{
		return m_game_objects;
	}

	std::weak_ptr<GameObject> Level::getGameObjectByID(size_t gobject_id)
	{
		if (m_game_objects.find(gobject_id) != m_game_objects.end())
		{
			return m_game_objects[gobject_id];
		}
		return std::weak_ptr<GameObject>();
	}

	std::shared_ptr<GameObject> Level::createGameObject(std::string name)
	{
		GameObjectID game_object_id = m_go_id_allocator.allocateId();
		std::shared_ptr<GameObject> game_object;
		try
		{
			game_object = std::make_shared<GameObject>(game_object_id);
		}
		catch (const std::exception& e)
		{
			std::cout << "can't create game object!" << std::endl;
		}
		game_object->m_name = name;
		m_game_objects.emplace(game_object_id, game_object);
		return game_object;
	}

	void Level::attachGObject(GameObjectID entity, GameObjectID parent, bool child_already_in_local_space)
	{
		assert(entity != parent);
		if (hierarchy_component_manager.contain(entity))
		{
			detach(entity);
		}
		HierarchyComponent & parent_component = hierarchy_component_manager.create(entity);
		parent_component.parent_id = parent;
		getGameObjectByID(entity).lock()->setParent(getGameObjectByID(parent).lock().get());
		getGameObjectByID(parent).lock()->addChild(getGameObjectByID(entity).lock().get());
		TransformComponent* parent_transform_component = getGameObjectByID(parent).lock()->TryGetComponent(TransformComponent);
		TransformComponent* transform_component = getGameObjectByID(entity).lock()->TryGetComponent(TransformComponent);
		if (parent_transform_component != nullptr && transform_component != nullptr)
		{
			if (!child_already_in_local_space)
			{
				
				Matrix4x4 inverse_matrix = parent_transform_component->world_matrix.inverse();
				transform_component->matrixTransform(inverse_matrix);
				transform_component->updateWorldMatrix();
			}
			transform_component->updateWorldMatrix_Parented(*parent_transform_component);

		}
	}

	bool Level::importModel_OBJ(std::string file_path)
	{
		ObjectInstanceResource instance_res;
		//initDefaultInstanceRes(instance_res);
		std::shared_ptr<GameObject> gobject = createGameObject(nullptr);
		gobject->m_name = Helper::getNameByFile(file_path) + "_" + std::to_string(gobject->getObjectId());
		gobject->TryAddComponent(TransformComponent);
		auto transform_component = gobject->TryGetComponent(TransformComponent);
		transform_component->postLoadResource(gobject);
		gobject->TryAddComponent(MeshComponent);
		auto *mesh_component = gobject->TryGetComponent(MeshComponent);
		SubMeshRes sub_mesh_res;
		sub_mesh_res.m_obj_file_ref = file_path;
		mesh_component->m_mesh_res.m_sub_meshes.push_back(sub_mesh_res);
		mesh_component->postLoadResource(gobject);
		return true;
	}

	bool Level::save()
	{
		LevelResource level_res;
		std::vector<ObjectInstanceResource>& output_object_instances = level_res.m_objects;
		level_res.m_character_name = m_current_active_character->getName();
		const size_t object_size = m_game_objects.size();
		output_object_instances.resize(object_size);
		unsigned int index = 0;
		for (const auto& id_object_pair : m_game_objects)
		{
			if (id_object_pair.second)
			{
				id_object_pair.second->save(output_object_instances[index]);
				++index;
			}
		}

		const bool is_save_success = g_runtime_global_context.m_asset_manager->saveAsset(level_res, m_current_level_url);
		if (is_save_success)
		{
			LOG_INFO("Success to save level!");
		}
		else
		{
			LOG_ERROR("Failed to save level");
		}
		return is_save_success;
	}

	void Level::detach(GameObjectID gobject)
	{
		const HierarchyComponent* hierarchy_component = hierarchy_component_manager.getComponent(gobject);
		if (hierarchy_component != nullptr)
		{
			TransformComponent* transform_component = getGameObjectByID(gobject).lock()->TryGetComponent(TransformComponent);
			if (transform_component)
			{
				transform_component->applyTransform();
			}

		}
		hierarchy_component_manager.remove(gobject);
	}
}