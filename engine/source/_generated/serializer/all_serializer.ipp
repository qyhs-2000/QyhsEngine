#pragma once
#include "_generated\serializer\component.serializer.gen.h"
#include "_generated\serializer\vector2.serializer.gen.h"
#include "_generated\serializer\quat.serializer.gen.h"
#include "_generated\serializer\mesh_component.serializer.gen.h"
#include "_generated\serializer\vector3.serializer.gen.h"
#include "_generated\serializer\transform.serializer.gen.h"
#include "_generated\serializer\vector4.serializer.gen.h"
#include "_generated\serializer\motor_component.serializer.gen.h"
#include "_generated\serializer\transform_component.serializer.gen.h"
#include "_generated\serializer\object.serializer.gen.h"
#include "_generated\serializer\mash_data.serializer.gen.h"
#include "_generated\serializer\mesh.serializer.gen.h"
#include "_generated\serializer\material.serializer.gen.h"
#include "_generated\serializer\level_resource.serializer.gen.h"
#include "_generated\serializer\world_resource.serializer.gen.h"
#include "_generated\serializer\camera_config.serializer.gen.h"
#include "_generated\serializer\global_rendering_resource.serializer.gen.h"
namespace QYHS
{
	template<>
	Component & Serializer::read(const Json & json_context,Component & instance)
	{
		assert(json_context.is_object());
		
		
		return instance;
	}
	template<>
	Vector2 & Serializer::read(const Json & json_context,Vector2 & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["x"].is_null())
		{
			Serializer::read(json_context["x"],instance.x);
		}
		if(!json_context["y"].is_null())
		{
			Serializer::read(json_context["y"],instance.y);
		}
		return instance;
	}
	template<>
	Quaternion & Serializer::read(const Json & json_context,Quaternion & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["w"].is_null())
		{
			Serializer::read(json_context["w"],instance.w);
		}
		if(!json_context["x"].is_null())
		{
			Serializer::read(json_context["x"],instance.x);
		}
		if(!json_context["y"].is_null())
		{
			Serializer::read(json_context["y"],instance.y);
		}
		if(!json_context["z"].is_null())
		{
			Serializer::read(json_context["z"],instance.z);
		}
		return instance;
	}
	template<>
	MeshComponent & Serializer::read(const Json & json_context,MeshComponent & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::Component*)&instance);
		if(!json_context["mesh_res"].is_null())
		{
			Serializer::read(json_context["mesh_res"],instance.m_mesh_res);
		}
		return instance;
	}
	template<>
	Vector3 & Serializer::read(const Json & json_context,Vector3 & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["x"].is_null())
		{
			Serializer::read(json_context["x"],instance.x);
		}
		if(!json_context["y"].is_null())
		{
			Serializer::read(json_context["y"],instance.y);
		}
		if(!json_context["z"].is_null())
		{
			Serializer::read(json_context["z"],instance.z);
		}
		return instance;
	}
	template<>
	Transform & Serializer::read(const Json & json_context,Transform & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["position"].is_null())
		{
			Serializer::read(json_context["position"],instance.m_position);
		}
		if(!json_context["scale"].is_null())
		{
			Serializer::read(json_context["scale"],instance.m_scale);
		}
		if(!json_context["rotation"].is_null())
		{
			Serializer::read(json_context["rotation"],instance.m_rotation);
		}
		return instance;
	}
	template<>
	Vector4 & Serializer::read(const Json & json_context,Vector4 & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["x"].is_null())
		{
			Serializer::read(json_context["x"],instance.x);
		}
		if(!json_context["y"].is_null())
		{
			Serializer::read(json_context["y"],instance.y);
		}
		if(!json_context["z"].is_null())
		{
			Serializer::read(json_context["z"],instance.z);
		}
		if(!json_context["w"].is_null())
		{
			Serializer::read(json_context["w"],instance.w);
		}
		return instance;
	}
	template<>
	MotorComponent & Serializer::read(const Json & json_context,MotorComponent & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::Component*)&instance);
		if(!json_context["transform"].is_null())
		{
			Serializer::read(json_context["transform"],instance.m_transform);
		}
		return instance;
	}
	template<>
	TransformComponent & Serializer::read(const Json & json_context,TransformComponent & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::Component*)&instance);
		if(!json_context["transform"].is_null())
		{
			Serializer::read(json_context["transform"],instance.m_transform);
		}
		return instance;
	}
	template<>
	ObjectDefinitionResource & Serializer::read(const Json & json_context,ObjectDefinitionResource & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["components"].is_null())
		{
			assert(json_context["components"].is_array());
			Json::array array_m_components = 
			json_context["components"].array_items();
			instance.m_components.resize(array_m_components.size());
			for(size_t index = 0;index<array_m_components.size();++index){
				Serializer::read(array_m_components[index],instance.m_components[index]);
			}
		}
		return instance;
	}
	template<>
	ObjectInstanceResource & Serializer::read(const Json & json_context,ObjectInstanceResource & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["name"].is_null())
		{
			Serializer::read(json_context["name"],instance.m_name);
		}
		if(!json_context["definition"].is_null())
		{
			Serializer::read(json_context["definition"],instance.m_definition);
		}
		if(!json_context["instanced_components"].is_null())
		{
			assert(json_context["instanced_components"].is_array());
			Json::array array_m_instanced_components = 
			json_context["instanced_components"].array_items();
			instance.m_instanced_components.resize(array_m_instanced_components.size());
			for(size_t index = 0;index<array_m_instanced_components.size();++index){
				Serializer::read(array_m_instanced_components[index],instance.m_instanced_components[index]);
			}
		}
		return instance;
	}
	template<>
	Vertex & Serializer::read(const Json & json_context,Vertex & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["px"].is_null())
		{
			Serializer::read(json_context["px"],instance.px);
		}
		if(!json_context["py"].is_null())
		{
			Serializer::read(json_context["py"],instance.py);
		}
		if(!json_context["pz"].is_null())
		{
			Serializer::read(json_context["pz"],instance.pz);
		}
		if(!json_context["u"].is_null())
		{
			Serializer::read(json_context["u"],instance.u);
		}
		if(!json_context["v"].is_null())
		{
			Serializer::read(json_context["v"],instance.v);
		}
		return instance;
	}
	template<>
	MeshData & Serializer::read(const Json & json_context,MeshData & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["vertex_buffer"].is_null())
		{
			assert(json_context["vertex_buffer"].is_array());
			Json::array array_vertex_buffer = 
			json_context["vertex_buffer"].array_items();
			instance.vertex_buffer.resize(array_vertex_buffer.size());
			for(size_t index = 0;index<array_vertex_buffer.size();++index){
				Serializer::read(array_vertex_buffer[index],instance.vertex_buffer[index]);
			}
		}
		if(!json_context["index_buffer"].is_null())
		{
			assert(json_context["index_buffer"].is_array());
			Json::array array_index_buffer = 
			json_context["index_buffer"].array_items();
			instance.index_buffer.resize(array_index_buffer.size());
			for(size_t index = 0;index<array_index_buffer.size();++index){
				Serializer::read(array_index_buffer[index],instance.index_buffer[index]);
			}
		}
		return instance;
	}
	template<>
	SubMeshRes & Serializer::read(const Json & json_context,SubMeshRes & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["material"].is_null())
		{
			Serializer::read(json_context["material"],instance.m_material);
		}
		if(!json_context["obj_file_ref"].is_null())
		{
			Serializer::read(json_context["obj_file_ref"],instance.m_obj_file_ref);
		}
		if(!json_context["transform"].is_null())
		{
			Serializer::read(json_context["transform"],instance.m_transform);
		}
		return instance;
	}
	template<>
	MeshComponentRes & Serializer::read(const Json & json_context,MeshComponentRes & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["sub_meshes"].is_null())
		{
			assert(json_context["sub_meshes"].is_array());
			Json::array array_m_sub_meshes = 
			json_context["sub_meshes"].array_items();
			instance.m_sub_meshes.resize(array_m_sub_meshes.size());
			for(size_t index = 0;index<array_m_sub_meshes.size();++index){
				Serializer::read(array_m_sub_meshes[index],instance.m_sub_meshes[index]);
			}
		}
		return instance;
	}
	template<>
	MaterialRes & Serializer::read(const Json & json_context,MaterialRes & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["base_colour_texture_file"].is_null())
		{
			Serializer::read(json_context["base_colour_texture_file"],instance.m_base_colour_texture_file);
		}
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
	template<>
	CameraPose & Serializer::read(const Json & json_context,CameraPose & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["position"].is_null())
		{
			Serializer::read(json_context["position"],instance.m_position);
		}
		if(!json_context["target"].is_null())
		{
			Serializer::read(json_context["target"],instance.m_target);
		}
		if(!json_context["up"].is_null())
		{
			Serializer::read(json_context["up"],instance.m_up);
		}
		return instance;
	}
	template<>
	CameraConfig & Serializer::read(const Json & json_context,CameraConfig & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["pose"].is_null())
		{
			Serializer::read(json_context["pose"],instance.m_pose);
		}
		if(!json_context["aspect"].is_null())
		{
			Serializer::read(json_context["aspect"],instance.m_aspect);
		}
		if(!json_context["z_far"].is_null())
		{
			Serializer::read(json_context["z_far"],instance.m_z_far);
		}
		if(!json_context["z_near"].is_null())
		{
			Serializer::read(json_context["z_near"],instance.m_z_near);
		}
		return instance;
	}
	template<>
	GlobalRenderConfig & Serializer::read(const Json & json_context,GlobalRenderConfig & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["camera_config"].is_null())
		{
			Serializer::read(json_context["camera_config"],instance.camera_config);
		}
		return instance;
	}
}

