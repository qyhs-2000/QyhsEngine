#include "game_object.h"
#include "resource/common/object.h"
#include "function/global/global_context.h"
#include "resource/asset_manager/asset_manager.h"
#include <memory>

namespace QYHS
{
	void GameObject::tick(double delta_time)
	{
		for (auto& component : m_components)
		{
			component->tick(delta_time);
		}
	}
	bool GameObject::load(const ObjectInstanceResource& instance_resource)
	{
		m_components.clear();

		m_components = instance_resource.m_instanced_components;
		for (auto& component : m_components)
		{
			if (component != nullptr)
			{
				component->postLoadResource(weak_from_this());
			}
		}

		//load object definition components
		m_definition_url = instance_resource.m_definition;
		ObjectDefinitionResource object_definition_resource;
		const bool load_object_definition_success = g_runtime_global_context.m_asset_manager->loadAsset(m_definition_url, object_definition_resource);
		assert(load_object_definition_success && "failed to load object definition resource");

		for (auto& component : object_definition_resource.m_components)
		{
			const std::string component_type_name = component.getTypeName();
			if (hasComponent(component_type_name))
				continue;
			component->postLoadResource(weak_from_this());
			m_components.emplace_back(component);
		}
		return true;
	}
	bool GameObject::hasComponent(std::string component_type_name)
	{
		for (auto& component : m_components)
		{
			if (component.getTypeName() == component_type_name)
			{
				return true;
			}
		}
		return false;
	}
}