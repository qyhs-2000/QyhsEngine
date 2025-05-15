#pragma once
#include <unordered_map>
#include <function/framework/game_object/game_object.h>
#include <function/framework/game_object/character.h>
#include "function/framework/component/hierarchy/hierarchy_component.h"
#include "function/framework/component/transform/transform_component.h"
#include "function/framework/component/mesh/mesh_component.h"
#include <function/framework/component/ecs.h>
#include <memory>
#include<string>
namespace qyhs
{
	class ObjectInstanceResource;
	class Level
	{
	public:
		ecs::ComponentLibrary component_library;
		ecs::ComponentManager<TransformComponent>& transform_componentmanager = component_library.Register<TransformComponent>("TransformComponent");
		ecs::ComponentManager<MeshComponent>& transform_component_manager = component_library.Register<MeshComponent>("TransformComponent");
		ecs::ComponentManager<HierarchyComponent>& hierarchy_component_manager = component_library.Register<HierarchyComponent>("HierarchyComponent");
		
		void tick(double delta_time);
		bool load(const std::string & level_url);
		std::shared_ptr<GameObject> createGameObject(ObjectInstanceResource * instance);
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>& getObjectsInLevel();
		std::weak_ptr<GameObject> getGameObjectByID(size_t gobject_id);
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>& getAllGameObjects() { return m_game_objects; }
		std::weak_ptr<Character> getCurrentActiveCharacter() { return m_current_active_character; }
		std::shared_ptr<GameObject> createGameObject(std::string name);
		void attachGObject(GameObjectID entity, GameObjectID parent, bool child_already_in_local_space);
		bool importModel_OBJ(std::string file_path);
		bool save();
		void detach(GameObjectID gobject);
	private:
		std::unordered_map<GameObjectID, std::shared_ptr<GameObject>>		m_game_objects;
		GameObjectIdAllocator	m_go_id_allocator;
		std::string m_current_level_url;
		std::shared_ptr<Character> m_current_active_character;
	};
}