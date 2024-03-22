#include "level.h"
#include <assert.h>
#include <iostream>
#include "resource/type/level_resource.h"
#include "resource/type/world_resource.h"
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
		createGameObject();
		return true;
	}
	void Level::createGameObject()
	{
		GameObjectID id = m_go_id_allocator.allocateId();
		std::shared_ptr<GameObject> game_object;
		try
		{
			game_object = std::make_shared<GameObject>();
		}
		catch (const std::exception& e)
		{
			std::cout << "can't create game object!" << std::endl;
		}
		game_object->load();
		m_game_objects.emplace(id, game_object);
	}
	std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>& Level::getObjectsInLevel()
	{
		return m_game_objects;
	}
}