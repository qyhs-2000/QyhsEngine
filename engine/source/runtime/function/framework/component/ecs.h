#pragma once
#include <string>
#include <unordered_map>
#include <memory>
#include <function/framework/game_object/game_object_id_allocator.h>
#include <cassert>
namespace qyhs::ecs
{
	using Entity = uint32_t;
	inline const Entity INVALID_ENTITY = 0;
	inline Entity createEntity()
	{
		static std::atomic<Entity> next{ INVALID_ENTITY + 1 };
		return next.fetch_add(1);
	}

	class ComponentManager_Interface
	{
	public:
		virtual void merge(ComponentManager_Interface& other) = 0;
	private:
	};

	template<typename TComponent>
	class ComponentManager :public ComponentManager_Interface
	{
	public:
		inline void merge(ComponentManager_Interface& other)
		{
			merge((ComponentManager<TComponent>&) other);
		}

		inline void merge(ComponentManager<TComponent>& other)
		{
			m_components.reserve(getCount() + other.getCount());
			m_entities.reserve(getCount() + other.getCount());
			lookup_map.reserve(getCount() + other.getCount());

			for (size_t i = 0; i < other.getCount(); ++i)
			{
				Entity entity = other.m_entities[i];
				assert(!contain(entity));
				m_entities.push_back(entity);
				lookup_map[entity] = m_components.size();
				m_components.push_back(std::move(other.m_components[i]));
			}

			other.clear();
		}

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

		uint32_t getIndex(Entity entity)const
		{
			if (lookup_map.empty())
			{
				return ~0u;
			}
			auto it = lookup_map.find(entity);
			if (it != lookup_map.end())
			{
				return it->second;
			}
			return ~0u;
		}

		inline void clear()
		{
			m_components.clear();
			m_entities.clear();
			lookup_map.clear();
		}

		uint32_t getCount()const
		{
			return m_components.size();
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

		inline TComponent& operator[](size_t index) { return m_components[index]; }
		inline const TComponent& operator[](size_t index)const { return m_components[index]; }
		//get entity by index
		ecs::Entity getEntity(int index)const { return m_entities[index]; }
	private:
		std::vector<GameObjectID> m_entities;
		std::vector<TComponent>   m_components;
		std::unordered_map<ecs::Entity, size_t> lookup_map;
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

		std::unordered_map<std::string, LibraryEntry> entries;
	private:
	};

}