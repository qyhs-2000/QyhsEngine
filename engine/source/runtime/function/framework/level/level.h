#pragma once
#include <unordered_map>
#include <function/framework/game_object/game_object.h>
#include <function/framework/game_object/character.h>
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
		std::weak_ptr<GameObject> getGameObjectByID(size_t gobject_id);
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>& getAllGameObjects() { return m_game_objects; }
		std::weak_ptr<Character> getCurrentActiveCharacter() { return m_current_active_character; }
		bool save();
	private:
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>		m_game_objects;
		GameObjectIdAllocator	m_go_id_allocator;
		std::string m_current_level_url;
		std::shared_ptr<Character> m_current_active_character;
	};
}