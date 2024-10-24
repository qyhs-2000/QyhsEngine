#include "level.h"
#include <assert.h>
#include <iostream>
#include "resource/type/level_resource.h"
#include "resource/type/world_resource.h"
#include "resource/common/object.h"
#include <resource/asset_manager/asset_manager.h>
namespace QYHS
{
	void Level::tick(double delta_time)
	{
		for (const auto& pair : m_game_objects)
		{
			assert(pair.second);
			pair.second->tick(delta_time);
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
			createGameObject(instance);
		}
		return true;
	}
	void Level::createGameObject(ObjectInstanceResource & instance_res)
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
}