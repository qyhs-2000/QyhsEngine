#pragma once
#include "_generated\serializer\component.serializer.gen.h"
#include "_generated\serializer\mesh_component.serializer.gen.h"
#include "_generated\serializer\object.serializer.gen.h"
#include "_generated\serializer\level_resource.serializer.gen.h"
#include "_generated\serializer\world_resource.serializer.gen.h"
namespace QYHS
{
	template<>
	Component & Serializer::read(const Json & json_context,Component & instance)
	{
		assert(json_context.is_object());
		
		
		return instance;
	}
	template<>
	MeshComponent & Serializer::read(const Json & json_context,MeshComponent & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::Component*)&instance);
		
		return instance;
	}
	template<>
	ObjectInstanceResource & Serializer::read(const Json & json_context,ObjectInstanceResource & instance)
	{
		assert(json_context.is_object());
		
		
		return instance;
	}
	template<>
	LevelResource & Serializer::read(const Json & json_context,LevelResource & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["gravity"].is_null())
		{
			Serializer::read(json_context["gravity"],instance.m_gravity);
		}
		if(!json_context["character_name"].is_null())
		{
			Serializer::read(json_context["character_name"],instance.m_character_name);
		}
		if(!json_context["objects"].is_null())
		{
			assert(json_context["objects"].is_array());
			Json::array array_m_objects = 
			json_context["objects"].array_items();
			instance.m_objects.resize(array_m_objects.size());
			for(size_t index = 0;index<array_m_objects.size();++index){
				Serializer::read(array_m_objects[index],instance.m_objects[index]);
			}
		}
		return instance;
	}
	template<>
	WorldResource & Serializer::read(const Json & json_context,WorldResource & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["default_level_url"].is_null())
		{
			Serializer::read(json_context["default_level_url"],instance.m_default_level_url);
		}
		return instance;
	}
}

