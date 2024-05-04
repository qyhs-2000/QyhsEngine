#pragma once
#include <unordered_map>
#include <function/framework/game_object/game_object.h>
#include <memory>
#include<string>
namespace QYHS
{
	class Level
	{
	public:
		void tick(double delta_time);
		bool load(const std::string & level_url);
		void createGameObject(ObjectInstanceResource& instance);
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>& getObjectsInLevel();
	private:
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>		m_game_objects;
		GameObjectIdAllocator	m_go_id_allocator;
		std::string m_current_level_url;
	};
}