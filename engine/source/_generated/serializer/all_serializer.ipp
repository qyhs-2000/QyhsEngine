#pragma once
#include "_generated\serializer\component.serializer.gen.h"
#include "_generated\serializer\vector2.serializer.gen.h"
#include "_generated\serializer\quat.serializer.gen.h"
#include "_generated\serializer\vector3.serializer.gen.h"
#include "_generated\serializer\transform.serializer.gen.h"
#include "_generated\serializer\vector4.serializer.gen.h"
#include "_generated\serializer\camera_config.serializer.gen.h"
#include "_generated\serializer\animation_component.serializer.gen.h"
#include "_generated\serializer\camera_component.serializer.gen.h"
#include "_generated\serializer\mesh_component.serializer.gen.h"
#include "_generated\serializer\motor_component.serializer.gen.h"
#include "_generated\serializer\transform_component.serializer.gen.h"
#include "_generated\serializer\object.serializer.gen.h"
#include "_generated\serializer\mash_data.serializer.gen.h"
#include "_generated\serializer\mesh.serializer.gen.h"
#include "_generated\serializer\material.serializer.gen.h"
#include "_generated\serializer\level_resource.serializer.gen.h"
#include "_generated\serializer\world_resource.serializer.gen.h"
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
    Json Serializer::write(const Component& instance){
        Json::object  ret_context;
        
        
        return  Json(ret_context);
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
    Json Serializer::write(const Vector2& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("x", Serializer::write(instance.x));
        ret_context.insert_or_assign("y", Serializer::write(instance.y));
        return  Json(ret_context);
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
    Json Serializer::write(const Quaternion& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("w", Serializer::write(instance.w));
        ret_context.insert_or_assign("x", Serializer::write(instance.x));
        ret_context.insert_or_assign("y", Serializer::write(instance.y));
        ret_context.insert_or_assign("z", Serializer::write(instance.z));
        return  Json(ret_context);
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
    Json Serializer::write(const Vector3& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("x", Serializer::write(instance.x));
        ret_context.insert_or_assign("y", Serializer::write(instance.y));
        ret_context.insert_or_assign("z", Serializer::write(instance.z));
        return  Json(ret_context);
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
    Json Serializer::write(const Transform& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("position", Serializer::write(instance.m_position));
        ret_context.insert_or_assign("scale", Serializer::write(instance.m_scale));
        ret_context.insert_or_assign("rotation", Serializer::write(instance.m_rotation));
        return  Json(ret_context);
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
    Json Serializer::write(const Vector4& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("x", Serializer::write(instance.x));
        ret_context.insert_or_assign("y", Serializer::write(instance.y));
        ret_context.insert_or_assign("z", Serializer::write(instance.z));
        ret_context.insert_or_assign("w", Serializer::write(instance.w));
        return  Json(ret_context);
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
    Json Serializer::write(const CameraPose& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("position", Serializer::write(instance.m_position));
        ret_context.insert_or_assign("target", Serializer::write(instance.m_target));
        ret_context.insert_or_assign("up", Serializer::write(instance.m_up));
        return  Json(ret_context);
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
    Json Serializer::write(const CameraConfig& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("pose", Serializer::write(instance.m_pose));
        ret_context.insert_or_assign("aspect", Serializer::write(instance.m_aspect));
        ret_context.insert_or_assign("z_far", Serializer::write(instance.m_z_far));
        ret_context.insert_or_assign("z_near", Serializer::write(instance.m_z_near));
        return  Json(ret_context);
    }


	template<>
	AnimationComponet & Serializer::read(const Json & json_context,AnimationComponet & instance)
	{
		assert(json_context.is_object());
		
		
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationComponet& instance){
        Json::object  ret_context;
        
        
        return  Json(ret_context);
    }


	template<>
	CameraParamter & Serializer::read(const Json & json_context,CameraParamter & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["fov"].is_null())
		{
			Serializer::read(json_context["fov"],instance.m_fov);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const CameraParamter& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("fov", Serializer::write(instance.m_fov));
        return  Json(ret_context);
    }


	template<>
	ThirdPersonCameraParameter & Serializer::read(const Json & json_context,ThirdPersonCameraParameter & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::CameraParamter*)&instance);
		if(!json_context["horizontal_offset"].is_null())
		{
			Serializer::read(json_context["horizontal_offset"],instance.m_horizontal_offset);
		}
		if(!json_context["vertical_offset"].is_null())
		{
			Serializer::read(json_context["vertical_offset"],instance.m_vertical_offset);
		}
		if(!json_context["cursor_pitch"].is_null())
		{
			Serializer::read(json_context["cursor_pitch"],instance.m_cursor_pitch);
		}
		if(!json_context["cursor_yaw"].is_null())
		{
			Serializer::read(json_context["cursor_yaw"],instance.m_cursor_yaw);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const ThirdPersonCameraParameter& instance){
        Json::object  ret_context;
        auto&&  json_context_0 = Serializer::write(*(QYHS::CameraParamter*)&instance);
        assert(json_context_0.is_object());
        auto&& json_context_map_0 = json_context_0.object_items();
        ret_context.insert(json_context_map_0.begin() , json_context_map_0.end());
        ret_context.insert_or_assign("horizontal_offset", Serializer::write(instance.m_horizontal_offset));
        ret_context.insert_or_assign("vertical_offset", Serializer::write(instance.m_vertical_offset));
        ret_context.insert_or_assign("cursor_pitch", Serializer::write(instance.m_cursor_pitch));
        ret_context.insert_or_assign("cursor_yaw", Serializer::write(instance.m_cursor_yaw));
        return  Json(ret_context);
    }


	template<>
	CameraComponentResource & Serializer::read(const Json & json_context,CameraComponentResource & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["parameter"].is_null())
		{
			Serializer::read(json_context["parameter"],instance.m_parameter);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const CameraComponentResource& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("parameter", Serializer::write(instance.m_parameter));
        return  Json(ret_context);
    }


	template<>
	CameraComponent & Serializer::read(const Json & json_context,CameraComponent & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::Component*)&instance);
		if(!json_context["camera_res"].is_null())
		{
			Serializer::read(json_context["camera_res"],instance.m_camera_res);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const CameraComponent& instance){
        Json::object  ret_context;
        auto&&  json_context_0 = Serializer::write(*(QYHS::Component*)&instance);
        assert(json_context_0.is_object());
        auto&& json_context_map_0 = json_context_0.object_items();
        ret_context.insert(json_context_map_0.begin() , json_context_map_0.end());
        ret_context.insert_or_assign("camera_res", Serializer::write(instance.m_camera_res));
        return  Json(ret_context);
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
    Json Serializer::write(const MeshComponent& instance){
        Json::object  ret_context;
        auto&&  json_context_0 = Serializer::write(*(QYHS::Component*)&instance);
        assert(json_context_0.is_object());
        auto&& json_context_map_0 = json_context_0.object_items();
        ret_context.insert(json_context_map_0.begin() , json_context_map_0.end());
        ret_context.insert_or_assign("mesh_res", Serializer::write(instance.m_mesh_res));
        return  Json(ret_context);
    }


	template<>
	MotorComponentRes & Serializer::read(const Json & json_context,MotorComponentRes & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["move_speed"].is_null())
		{
			Serializer::read(json_context["move_speed"],instance.move_speed);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const MotorComponentRes& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("move_speed", Serializer::write(instance.move_speed));
        return  Json(ret_context);
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
		if(!json_context["motor_res"].is_null())
		{
			Serializer::read(json_context["motor_res"],instance.m_motor_res);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const MotorComponent& instance){
        Json::object  ret_context;
        auto&&  json_context_0 = Serializer::write(*(QYHS::Component*)&instance);
        assert(json_context_0.is_object());
        auto&& json_context_map_0 = json_context_0.object_items();
        ret_context.insert(json_context_map_0.begin() , json_context_map_0.end());
        ret_context.insert_or_assign("transform", Serializer::write(instance.m_transform));
        ret_context.insert_or_assign("motor_res", Serializer::write(instance.m_motor_res));
        return  Json(ret_context);
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
    Json Serializer::write(const TransformComponent& instance){
        Json::object  ret_context;
        auto&&  json_context_0 = Serializer::write(*(QYHS::Component*)&instance);
        assert(json_context_0.is_object());
        auto&& json_context_map_0 = json_context_0.object_items();
        ret_context.insert(json_context_map_0.begin() , json_context_map_0.end());
        ret_context.insert_or_assign("transform", Serializer::write(instance.m_transform));
        return  Json(ret_context);
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
    Json Serializer::write(const ObjectDefinitionResource& instance){
        Json::object  ret_context;
        
        Json::array m_components_json;
        for (auto& item : instance.m_components){
            m_components_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("components",m_components_json);
        
        return  Json(ret_context);
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
    Json Serializer::write(const ObjectInstanceResource& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("name", Serializer::write(instance.m_name));
        ret_context.insert_or_assign("definition", Serializer::write(instance.m_definition));
        Json::array m_instanced_components_json;
        for (auto& item : instance.m_instanced_components){
            m_instanced_components_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("instanced_components",m_instanced_components_json);
        
        return  Json(ret_context);
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
		if(!json_context["nx"].is_null())
		{
			Serializer::read(json_context["nx"],instance.nx);
		}
		if(!json_context["ny"].is_null())
		{
			Serializer::read(json_context["ny"],instance.ny);
		}
		if(!json_context["nz"].is_null())
		{
			Serializer::read(json_context["nz"],instance.nz);
		}
		if(!json_context["tx"].is_null())
		{
			Serializer::read(json_context["tx"],instance.tx);
		}
		if(!json_context["ty"].is_null())
		{
			Serializer::read(json_context["ty"],instance.ty);
		}
		if(!json_context["tz"].is_null())
		{
			Serializer::read(json_context["tz"],instance.tz);
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
    Json Serializer::write(const Vertex& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("px", Serializer::write(instance.px));
        ret_context.insert_or_assign("py", Serializer::write(instance.py));
        ret_context.insert_or_assign("pz", Serializer::write(instance.pz));
        ret_context.insert_or_assign("nx", Serializer::write(instance.nx));
        ret_context.insert_or_assign("ny", Serializer::write(instance.ny));
        ret_context.insert_or_assign("nz", Serializer::write(instance.nz));
        ret_context.insert_or_assign("tx", Serializer::write(instance.tx));
        ret_context.insert_or_assign("ty", Serializer::write(instance.ty));
        ret_context.insert_or_assign("tz", Serializer::write(instance.tz));
        ret_context.insert_or_assign("u", Serializer::write(instance.u));
        ret_context.insert_or_assign("v", Serializer::write(instance.v));
        return  Json(ret_context);
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
    Json Serializer::write(const MeshData& instance){
        Json::object  ret_context;
        
        Json::array vertex_buffer_json;
        for (auto& item : instance.vertex_buffer){
            vertex_buffer_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("vertex_buffer",vertex_buffer_json);
        
        Json::array index_buffer_json;
        for (auto& item : instance.index_buffer){
            index_buffer_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("index_buffer",index_buffer_json);
        
        return  Json(ret_context);
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
    Json Serializer::write(const SubMeshRes& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("material", Serializer::write(instance.m_material));
        ret_context.insert_or_assign("obj_file_ref", Serializer::write(instance.m_obj_file_ref));
        ret_context.insert_or_assign("transform", Serializer::write(instance.m_transform));
        return  Json(ret_context);
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
    Json Serializer::write(const MeshComponentRes& instance){
        Json::object  ret_context;
        
        Json::array m_sub_meshes_json;
        for (auto& item : instance.m_sub_meshes){
            m_sub_meshes_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("sub_meshes",m_sub_meshes_json);
        
        return  Json(ret_context);
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
    Json Serializer::write(const MaterialRes& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("base_colour_texture_file", Serializer::write(instance.m_base_colour_texture_file));
        return  Json(ret_context);
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
    Json Serializer::write(const LevelResource& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("gravity", Serializer::write(instance.m_gravity));
        ret_context.insert_or_assign("character_name", Serializer::write(instance.m_character_name));
        Json::array m_objects_json;
        for (auto& item : instance.m_objects){
            m_objects_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("objects",m_objects_json);
        
        return  Json(ret_context);
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
    Json Serializer::write(const WorldResource& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("default_level_url", Serializer::write(instance.m_default_level_url));
        return  Json(ret_context);
    }


	template<>
	SkyBoxSpecularMap & Serializer::read(const Json & json_context,SkyBoxSpecularMap & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["positive_x_map"].is_null())
		{
			Serializer::read(json_context["positive_x_map"],instance.m_positive_x_map);
		}
		if(!json_context["negative_x_map"].is_null())
		{
			Serializer::read(json_context["negative_x_map"],instance.m_negative_x_map);
		}
		if(!json_context["positive_y_map"].is_null())
		{
			Serializer::read(json_context["positive_y_map"],instance.m_positive_y_map);
		}
		if(!json_context["negative_y_map"].is_null())
		{
			Serializer::read(json_context["negative_y_map"],instance.m_negative_y_map);
		}
		if(!json_context["positive_z_map"].is_null())
		{
			Serializer::read(json_context["positive_z_map"],instance.m_positive_z_map);
		}
		if(!json_context["negative_z_map"].is_null())
		{
			Serializer::read(json_context["negative_z_map"],instance.m_negative_z_map);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const SkyBoxSpecularMap& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("positive_x_map", Serializer::write(instance.m_positive_x_map));
        ret_context.insert_or_assign("negative_x_map", Serializer::write(instance.m_negative_x_map));
        ret_context.insert_or_assign("positive_y_map", Serializer::write(instance.m_positive_y_map));
        ret_context.insert_or_assign("negative_y_map", Serializer::write(instance.m_negative_y_map));
        ret_context.insert_or_assign("positive_z_map", Serializer::write(instance.m_positive_z_map));
        ret_context.insert_or_assign("negative_z_map", Serializer::write(instance.m_negative_z_map));
        return  Json(ret_context);
    }


	template<>
	GlobalRenderConfig & Serializer::read(const Json & json_context,GlobalRenderConfig & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["camera_config"].is_null())
		{
			Serializer::read(json_context["camera_config"],instance.camera_config);
		}
		if(!json_context["skybox_specular_map"].is_null())
		{
			Serializer::read(json_context["skybox_specular_map"],instance.m_skybox_specular_map);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const GlobalRenderConfig& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("camera_config", Serializer::write(instance.camera_config));
        ret_context.insert_or_assign("skybox_specular_map", Serializer::write(instance.m_skybox_specular_map));
        return  Json(ret_context);
    }


}

