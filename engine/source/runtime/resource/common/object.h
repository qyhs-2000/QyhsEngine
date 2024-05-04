#pragma once
#include "core/meta/reflection/reflection.h"
#include "runtime/function/framework/component/component.h"
namespace QYHS
{
	REFLECTION_TYPE(ObjectDefinitionResource)
		CLASS(ObjectDefinitionResource, Fields)
	{
	public:
		std::vector<Reflection::ReflectionPtr<Component>> m_components;
	private:
	};

	REFLECTION_TYPE(ObjectInstanceResource)
	CLASS(ObjectInstanceResource,Fields)
	{
	public:
		std::string					  m_name;
		std::string					  m_definition;
		std::vector<Reflection::ReflectionPtr<Component>> m_instanced_components;
	private:
	};
}