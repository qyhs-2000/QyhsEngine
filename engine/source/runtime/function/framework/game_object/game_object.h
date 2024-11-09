#pragma once
#include <vector>
#include "game_object_id_allocator.h"
#include <function/framework/component/component.h>
#include <core/meta/reflection/reflection.h>
#include <memory>
#include <memory.h>
namespace QYHS
{
	class Component;
	class GameObject :public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject(GameObjectID id) :m_id{ id } {}
		void tick(double delta_time);
		GameObjectID getObjectId() { return m_id; }
		bool load(const class ObjectInstanceResource& instance_resource);
		bool hasComponent(std::string component_type_name);
		template<typename TComponent>
		TComponent* tryGetComponent(const std::string& component_type_name)
		{
			for (auto component : m_components)
			{
				if (component.getTypeName() == component_type_name)
				{
					return static_cast<TComponent*>(component.operator->());
				}
			}
			return nullptr;
		}
		std::string m_name;
		std::vector<Reflection::ReflectionPtr<Component>> &getComponents() { return m_components; }
#define TryGetComponent(component_type) tryGetComponent<component_type>(#component_type);
	private:
		std::vector<Reflection::ReflectionPtr<Component>> m_components;
		GameObjectID	m_id;
		std::string m_definition_url;
	};
}