#pragma once
#include <vector>
#include "game_object_id_allocator.h"
#include <function/framework/component/component.h>
#include <core/meta/reflection/reflection.h>
#include <memory>
#include <memory.h>
#include <string>
#include "core/utils/utils.h"
namespace QYHS
{
	class Component;
	class GameObject :public std::enable_shared_from_this<GameObject>
	{
	public:
		GameObject(GameObjectID id) :m_id{ id } {}
		void tick(double delta_time);
		void setParent(GameObject* p_parent) { m_parent = p_parent; }
		GameObjectID getObjectId() { return m_id; }
		bool load(const class ObjectInstanceResource *  instance_resource);
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

		template<typename TComponent>
		bool tryAddComponent(std::string component_type,TComponent * p_component = nullptr)
		{
			if (p_component == nullptr)
			{
				TComponent* component = new TComponent;
				m_components.push_back(Reflection::ReflectionPtr<Component>(component_type,component));
				m_components.back()->setParent(shared_from_this());
				return true;
			}
			else
			{
				m_components.push_back(Reflection::ReflectionPtr<Component>(component_type,p_component));
				m_components.back()->setParent(shared_from_this());
				return true;
			}
		}
		GameObject* getParent()const { return m_parent; }
		void save(class ObjectInstanceResource& instance_res);
		std::string m_name;
		std::vector<Reflection::ReflectionPtr<Component>> &getComponents() { return m_components; }
		bool shouldComponentTick(std::string component_type_name);
#define TryGetComponent(component_type) tryGetComponent<component_type>(#component_type);
#define TryAddComponent(component_type) tryAddComponent<component_type>(#component_type,nullptr);
#define TryAddComponentWithInstance(component_type,p_component) tryAddComponent<component_type>(#component_type,p_component);
	private:
		std::vector<Reflection::ReflectionPtr<Component>> m_components{};
		GameObjectID	m_id;
		std::string m_definition_url;
		std::vector<GameObject* > childerens;
		GameObject* m_parent{ nullptr };
	};
}