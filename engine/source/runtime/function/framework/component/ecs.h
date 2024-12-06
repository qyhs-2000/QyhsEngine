#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <function/framework/game_object/game_object_id_allocator.h>
#include <cassert>
namespace QYHS
{

	class ComponentManager_Interface
	{
	public:

	private:
	};

	template<typename TComponent>
	class ComponentManager :public ComponentManager_Interface
	{
	public:
		bool contain(GameObjectID gobject_id)
		{
			for (auto& entity : m_entities)
			{
				if (gobject_id == entity)
				{
					return true;
				}
			}
			return false;
		}

		void remove(GameObjectID gobject)
		{
			auto iter = lookup_map.find(gobject);
			if (iter != lookup_map.end())
			{
				const size_t index = iter->second;
				const GameObjectID entity = m_entities[index];
				if (index < m_components.size() - 1)
				{
					m_components[index] = std::move(m_components.back());
					m_entities[index] = m_entities.back();

					lookup_map[m_entities[index]] = index;
				}
				m_components.pop_back();
				m_entities.pop_back();
				lookup_map.erase(entity);
			}
		}

		TComponent& create(GameObjectID gobject)
		{
			assert(gobject != k_invalid_gobject_id);
			assert(m_components.size() == m_entities.size());
			assert(lookup_map.size() == m_components.size());

			lookup_map[gobject] = m_components.size();
			m_components.emplace_back();
			m_entities.push_back(gobject);
			return m_components.back();
		}

		inline TComponent* getComponent(GameObjectID gobject)
		{
			auto iter = lookup_map.find(gobject);
			if (iter == lookup_map.end())
			{
				return nullptr;
			}
			return &m_components[iter->second];
		}

	private:
		std::vector<GameObjectID> m_entities;
		std::vector<TComponent>   m_components;
		std::unordered_map<GameObjectID, size_t> lookup_map;
	};

	class ComponentLibrary
	{
	public:
		struct LibraryEntry
		{
			std::unique_ptr<ComponentManager_Interface> component_manager;
		};
		template<typename TComponent>
		ComponentManager<TComponent>& Register(const std::string& name)
		{
			entries[name].component_manager = std::make_unique<ComponentManager<TComponent>>();
			return static_cast<ComponentManager<TComponent>&>(*entries[name].component_manager);
		}

	private:
		std::unordered_map<std::string, LibraryEntry> entries;
	};

}