#pragma once
#include "_generated\serializer\vector2.serializer.gen.h"
#include "_generated\serializer\matrix4.serializer.gen.h"
#include "_generated\serializer\quat.serializer.gen.h"
#include "_generated\serializer\vector3.serializer.gen.h"
#include "_generated\serializer\transform.serializer.gen.h"
#include "_generated\serializer\animation_clip.serializer.gen.h"
#include "_generated\serializer\vector4.serializer.gen.h"
#include "_generated\serializer\motor_component.serializer.gen.h"
#include "_generated\serializer\animation.serializer.gen.h"
#include "_generated\serializer\animation_skeleton_map.serializer.gen.h"
#include "_generated\serializer\animation_component.serializer.gen.h"
#include "_generated\serializer\animation_mask.serializer.gen.h"
#include "_generated\serializer\component.serializer.gen.h"
#include "_generated\serializer\camera_component.serializer.gen.h"
#include "_generated\serializer\hierarchy_component.serializer.gen.h"
#include "_generated\serializer\mesh_component.serializer.gen.h"
#include "_generated\serializer\transform_component.serializer.gen.h"
#include "_generated\serializer\camera_config.serializer.gen.h"
#include "_generated\serializer\object.serializer.gen.h"
#include "_generated\serializer\mash_data.serializer.gen.h"
#include "_generated\serializer\mesh.serializer.gen.h"
#include "_generated\serializer\material.serializer.gen.h"
#include "_generated\serializer\level_resource.serializer.gen.h"
#include "_generated\serializer\world_resource.serializer.gen.h"
#include "_generated\serializer\global_rendering_resource.serializer.gen.h"
#include "_generated\serializer\skeleton_data.serializer.gen.h"
namespace QYHS
{
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
	TmpTestClass & Serializer::read(const Json & json_context,TmpTestClass & instance)
	{
		assert(json_context.is_object());
		
		
		return instance;
	}

	template<>
    Json Serializer::write(const TmpTestClass& instance){
        Json::object  ret_context;
        
        
        return  Json(ret_context);
    }


	template<>
	Matrix4x4_ & Serializer::read(const Json & json_context,Matrix4x4_ & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["v0"].is_null())
		{
			Serializer::read(json_context["v0"],instance.v0);
		}
		if(!json_context["v1"].is_null())
		{
			Serializer::read(json_context["v1"],instance.v1);
		}
		if(!json_context["v2"].is_null())
		{
			Serializer::read(json_context["v2"],instance.v2);
		}
		if(!json_context["v3"].is_null())
		{
			Serializer::read(json_context["v3"],instance.v3);
		}
		if(!json_context["v4"].is_null())
		{
			Serializer::read(json_context["v4"],instance.v4);
		}
		if(!json_context["v5"].is_null())
		{
			Serializer::read(json_context["v5"],instance.v5);
		}
		if(!json_context["v6"].is_null())
		{
			Serializer::read(json_context["v6"],instance.v6);
		}
		if(!json_context["v7"].is_null())
		{
			Serializer::read(json_context["v7"],instance.v7);
		}
		if(!json_context["v8"].is_null())
		{
			Serializer::read(json_context["v8"],instance.v8);
		}
		if(!json_context["v9"].is_null())
		{
			Serializer::read(json_context["v9"],instance.v9);
		}
		if(!json_context["v10"].is_null())
		{
			Serializer::read(json_context["v10"],instance.v10);
		}
		if(!json_context["v11"].is_null())
		{
			Serializer::read(json_context["v11"],instance.v11);
		}
		if(!json_context["v12"].is_null())
		{
			Serializer::read(json_context["v12"],instance.v12);
		}
		if(!json_context["v13"].is_null())
		{
			Serializer::read(json_context["v13"],instance.v13);
		}
		if(!json_context["v14"].is_null())
		{
			Serializer::read(json_context["v14"],instance.v14);
		}
		if(!json_context["v15"].is_null())
		{
			Serializer::read(json_context["v15"],instance.v15);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const Matrix4x4_& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("v0", Serializer::write(instance.v0));
        ret_context.insert_or_assign("v1", Serializer::write(instance.v1));
        ret_context.insert_or_assign("v2", Serializer::write(instance.v2));
        ret_context.insert_or_assign("v3", Serializer::write(instance.v3));
        ret_context.insert_or_assign("v4", Serializer::write(instance.v4));
        ret_context.insert_or_assign("v5", Serializer::write(instance.v5));
        ret_context.insert_or_assign("v6", Serializer::write(instance.v6));
        ret_context.insert_or_assign("v7", Serializer::write(instance.v7));
        ret_context.insert_or_assign("v8", Serializer::write(instance.v8));
        ret_context.insert_or_assign("v9", Serializer::write(instance.v9));
        ret_context.insert_or_assign("v10", Serializer::write(instance.v10));
        ret_context.insert_or_assign("v11", Serializer::write(instance.v11));
        ret_context.insert_or_assign("v12", Serializer::write(instance.v12));
        ret_context.insert_or_assign("v13", Serializer::write(instance.v13));
        ret_context.insert_or_assign("v14", Serializer::write(instance.v14));
        ret_context.insert_or_assign("v15", Serializer::write(instance.v15));
        return  Json(ret_context);
    }


	template<>
	TmpTest & Serializer::read(const Json & json_context,TmpTest & instance)
	{
		assert(json_context.is_object());
		
		
		return instance;
	}

	template<>
    Json Serializer::write(const TmpTest& instance){
        Json::object  ret_context;
        
        
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
	AnimationNodeMap & Serializer::read(const Json & json_context,AnimationNodeMap & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["convert"].is_null())
		{
			assert(json_context["convert"].is_array());
			Json::array array_convert = 
			json_context["convert"].array_items();
			instance.convert.resize(array_convert.size());
			for(size_t index = 0;index<array_convert.size();++index){
				Serializer::read(array_convert[index],instance.convert[index]);
			}
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationNodeMap& instance){
        Json::object  ret_context;
        
        Json::array convert_json;
        for (auto& item : instance.convert){
            convert_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("convert",convert_json);
        
        return  Json(ret_context);
    }


	template<>
	AnimationChannel & Serializer::read(const Json & json_context,AnimationChannel & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["name"].is_null())
		{
			Serializer::read(json_context["name"],instance.name);
		}
		if(!json_context["position_keys"].is_null())
		{
			assert(json_context["position_keys"].is_array());
			Json::array array_position_keys = 
			json_context["position_keys"].array_items();
			instance.position_keys.resize(array_position_keys.size());
			for(size_t index = 0;index<array_position_keys.size();++index){
				Serializer::read(array_position_keys[index],instance.position_keys[index]);
			}
		}
		if(!json_context["rotation_keys"].is_null())
		{
			assert(json_context["rotation_keys"].is_array());
			Json::array array_rotation_keys = 
			json_context["rotation_keys"].array_items();
			instance.rotation_keys.resize(array_rotation_keys.size());
			for(size_t index = 0;index<array_rotation_keys.size();++index){
				Serializer::read(array_rotation_keys[index],instance.rotation_keys[index]);
			}
		}
		if(!json_context["scaling_keys"].is_null())
		{
			assert(json_context["scaling_keys"].is_array());
			Json::array array_scaling_keys = 
			json_context["scaling_keys"].array_items();
			instance.scaling_keys.resize(array_scaling_keys.size());
			for(size_t index = 0;index<array_scaling_keys.size();++index){
				Serializer::read(array_scaling_keys[index],instance.scaling_keys[index]);
			}
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationChannel& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("name", Serializer::write(instance.name));
        Json::array position_keys_json;
        for (auto& item : instance.position_keys){
            position_keys_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("position_keys",position_keys_json);
        
        Json::array rotation_keys_json;
        for (auto& item : instance.rotation_keys){
            rotation_keys_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("rotation_keys",rotation_keys_json);
        
        Json::array scaling_keys_json;
        for (auto& item : instance.scaling_keys){
            scaling_keys_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("scaling_keys",scaling_keys_json);
        
        return  Json(ret_context);
    }


	template<>
	AnimationClip & Serializer::read(const Json & json_context,AnimationClip & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["total_frame"].is_null())
		{
			Serializer::read(json_context["total_frame"],instance.total_frame);
		}
		if(!json_context["node_count"].is_null())
		{
			Serializer::read(json_context["node_count"],instance.node_count);
		}
		if(!json_context["node_channels"].is_null())
		{
			assert(json_context["node_channels"].is_array());
			Json::array array_node_channels = 
			json_context["node_channels"].array_items();
			instance.node_channels.resize(array_node_channels.size());
			for(size_t index = 0;index<array_node_channels.size();++index){
				Serializer::read(array_node_channels[index],instance.node_channels[index]);
			}
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationClip& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("total_frame", Serializer::write(instance.total_frame));
        ret_context.insert_or_assign("node_count", Serializer::write(instance.node_count));
        Json::array node_channels_json;
        for (auto& item : instance.node_channels){
            node_channels_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("node_channels",node_channels_json);
        
        return  Json(ret_context);
    }


	template<>
	AnimationAsset & Serializer::read(const Json & json_context,AnimationAsset & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["node_map"].is_null())
		{
			Serializer::read(json_context["node_map"],instance.node_map);
		}
		if(!json_context["clip_data"].is_null())
		{
			Serializer::read(json_context["clip_data"],instance.clip_data);
		}
		if(!json_context["skeleton_file_path"].is_null())
		{
			Serializer::read(json_context["skeleton_file_path"],instance.skeleton_file_path);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationAsset& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("node_map", Serializer::write(instance.node_map));
        ret_context.insert_or_assign("clip_data", Serializer::write(instance.clip_data));
        ret_context.insert_or_assign("skeleton_file_path", Serializer::write(instance.skeleton_file_path));
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
	BlendState & Serializer::read(const Json & json_context,BlendState & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["blend_anim_skel_map_path"].is_null())
		{
			assert(json_context["blend_anim_skel_map_path"].is_array());
			Json::array array_blend_anim_skel_map_path = 
			json_context["blend_anim_skel_map_path"].array_items();
			instance.blend_anim_skel_map_path.resize(array_blend_anim_skel_map_path.size());
			for(size_t index = 0;index<array_blend_anim_skel_map_path.size();++index){
				Serializer::read(array_blend_anim_skel_map_path[index],instance.blend_anim_skel_map_path[index]);
			}
		}
		if(!json_context["blend_clip_file_path"].is_null())
		{
			assert(json_context["blend_clip_file_path"].is_array());
			Json::array array_blend_clip_file_path = 
			json_context["blend_clip_file_path"].array_items();
			instance.blend_clip_file_path.resize(array_blend_clip_file_path.size());
			for(size_t index = 0;index<array_blend_clip_file_path.size();++index){
				Serializer::read(array_blend_clip_file_path[index],instance.blend_clip_file_path[index]);
			}
		}
		if(!json_context["blend_mask_file_path"].is_null())
		{
			assert(json_context["blend_mask_file_path"].is_array());
			Json::array array_blend_mask_file_path = 
			json_context["blend_mask_file_path"].array_items();
			instance.blend_mask_file_path.resize(array_blend_mask_file_path.size());
			for(size_t index = 0;index<array_blend_mask_file_path.size();++index){
				Serializer::read(array_blend_mask_file_path[index],instance.blend_mask_file_path[index]);
			}
		}
		if(!json_context["blend_clip_file_length"].is_null())
		{
			assert(json_context["blend_clip_file_length"].is_array());
			Json::array array_blend_clip_file_length = 
			json_context["blend_clip_file_length"].array_items();
			instance.blend_clip_file_length.resize(array_blend_clip_file_length.size());
			for(size_t index = 0;index<array_blend_clip_file_length.size();++index){
				Serializer::read(array_blend_clip_file_length[index],instance.blend_clip_file_length[index]);
			}
		}
		if(!json_context["blend_ratio"].is_null())
		{
			assert(json_context["blend_ratio"].is_array());
			Json::array array_blend_ratio = 
			json_context["blend_ratio"].array_items();
			instance.blend_ratio.resize(array_blend_ratio.size());
			for(size_t index = 0;index<array_blend_ratio.size();++index){
				Serializer::read(array_blend_ratio[index],instance.blend_ratio[index]);
			}
		}
		if(!json_context["blend_weight"].is_null())
		{
			assert(json_context["blend_weight"].is_array());
			Json::array array_blend_weight = 
			json_context["blend_weight"].array_items();
			instance.blend_weight.resize(array_blend_weight.size());
			for(size_t index = 0;index<array_blend_weight.size();++index){
				Serializer::read(array_blend_weight[index],instance.blend_weight[index]);
			}
		}
		if(!json_context["clip_count"].is_null())
		{
			Serializer::read(json_context["clip_count"],instance.clip_count);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const BlendState& instance){
        Json::object  ret_context;
        
        Json::array blend_anim_skel_map_path_json;
        for (auto& item : instance.blend_anim_skel_map_path){
            blend_anim_skel_map_path_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("blend_anim_skel_map_path",blend_anim_skel_map_path_json);
        
        Json::array blend_clip_file_path_json;
        for (auto& item : instance.blend_clip_file_path){
            blend_clip_file_path_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("blend_clip_file_path",blend_clip_file_path_json);
        
        Json::array blend_mask_file_path_json;
        for (auto& item : instance.blend_mask_file_path){
            blend_mask_file_path_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("blend_mask_file_path",blend_mask_file_path_json);
        
        Json::array blend_clip_file_length_json;
        for (auto& item : instance.blend_clip_file_length){
            blend_clip_file_length_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("blend_clip_file_length",blend_clip_file_length_json);
        
        Json::array blend_ratio_json;
        for (auto& item : instance.blend_ratio){
            blend_ratio_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("blend_ratio",blend_ratio_json);
        
        Json::array blend_weight_json;
        for (auto& item : instance.blend_weight){
            blend_weight_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("blend_weight",blend_weight_json);
        
        ret_context.insert_or_assign("clip_count", Serializer::write(instance.clip_count));
        return  Json(ret_context);
    }


	template<>
	AnimationComponentRes & Serializer::read(const Json & json_context,AnimationComponentRes & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["skeleton_file_path"].is_null())
		{
			Serializer::read(json_context["skeleton_file_path"],instance.skeleton_file_path);
		}
		if(!json_context["frame_position"].is_null())
		{
			Serializer::read(json_context["frame_position"],instance.frame_position);
		}
		if(!json_context["blend_state"].is_null())
		{
			Serializer::read(json_context["blend_state"],instance.blend_state);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationComponentRes& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("skeleton_file_path", Serializer::write(instance.skeleton_file_path));
        ret_context.insert_or_assign("frame_position", Serializer::write(instance.frame_position));
        ret_context.insert_or_assign("blend_state", Serializer::write(instance.blend_state));
        return  Json(ret_context);
    }


	template<>
	AnimationSkeletonMap & Serializer::read(const Json & json_context,AnimationSkeletonMap & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["convert"].is_null())
		{
			assert(json_context["convert"].is_array());
			Json::array array_convert = 
			json_context["convert"].array_items();
			instance.convert.resize(array_convert.size());
			for(size_t index = 0;index<array_convert.size();++index){
				Serializer::read(array_convert[index],instance.convert[index]);
			}
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationSkeletonMap& instance){
        Json::object  ret_context;
        
        Json::array convert_json;
        for (auto& item : instance.convert){
            convert_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("convert",convert_json);
        
        return  Json(ret_context);
    }


	template<>
	AnimationComponent & Serializer::read(const Json & json_context,AnimationComponent & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["animation_res"].is_null())
		{
			Serializer::read(json_context["animation_res"],instance.m_animation_res);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const AnimationComponent& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("animation_res", Serializer::write(instance.m_animation_res));
        return  Json(ret_context);
    }


	template<>
	BoneBlendMask & Serializer::read(const Json & json_context,BoneBlendMask & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["skeleton_file_path"].is_null())
		{
			Serializer::read(json_context["skeleton_file_path"],instance.skeleton_file_path);
		}
		if(!json_context["enabled"].is_null())
		{
			assert(json_context["enabled"].is_array());
			Json::array array_enabled = 
			json_context["enabled"].array_items();
			instance.enabled.resize(array_enabled.size());
			for(size_t index = 0;index<array_enabled.size();++index){
				Serializer::read(array_enabled[index],instance.enabled[index]);
			}
		}
		return instance;
	}

	template<>
    Json Serializer::write(const BoneBlendMask& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("skeleton_file_path", Serializer::write(instance.skeleton_file_path));
        Json::array enabled_json;
        for (auto& item : instance.enabled){
            enabled_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("enabled",enabled_json);
        
        return  Json(ret_context);
    }


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
	HierarchyComponent & Serializer::read(const Json & json_context,HierarchyComponent & instance)
	{
		assert(json_context.is_object());
		Serializer::read(json_context,*(QYHS::Component*)&instance);
		
		return instance;
	}

	template<>
    Json Serializer::write(const HierarchyComponent& instance){
        Json::object  ret_context;
        auto&&  json_context_0 = Serializer::write(*(QYHS::Component*)&instance);
        assert(json_context_0.is_object());
        auto&& json_context_map_0 = json_context_0.object_items();
        ret_context.insert(json_context_map_0.begin() , json_context_map_0.end());
        
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
	SkeletonBind & Serializer::read(const Json & json_context,SkeletonBind & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["index0"].is_null())
		{
			Serializer::read(json_context["index0"],instance.index0);
		}
		if(!json_context["index1"].is_null())
		{
			Serializer::read(json_context["index1"],instance.index1);
		}
		if(!json_context["index2"].is_null())
		{
			Serializer::read(json_context["index2"],instance.index2);
		}
		if(!json_context["index3"].is_null())
		{
			Serializer::read(json_context["index3"],instance.index3);
		}
		if(!json_context["weight0"].is_null())
		{
			Serializer::read(json_context["weight0"],instance.weight0);
		}
		if(!json_context["weight1"].is_null())
		{
			Serializer::read(json_context["weight1"],instance.weight1);
		}
		if(!json_context["weight2"].is_null())
		{
			Serializer::read(json_context["weight2"],instance.weight2);
		}
		if(!json_context["weight3"].is_null())
		{
			Serializer::read(json_context["weight3"],instance.weight3);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const SkeletonBind& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("index0", Serializer::write(instance.index0));
        ret_context.insert_or_assign("index1", Serializer::write(instance.index1));
        ret_context.insert_or_assign("index2", Serializer::write(instance.index2));
        ret_context.insert_or_assign("index3", Serializer::write(instance.index3));
        ret_context.insert_or_assign("weight0", Serializer::write(instance.weight0));
        ret_context.insert_or_assign("weight1", Serializer::write(instance.weight1));
        ret_context.insert_or_assign("weight2", Serializer::write(instance.weight2));
        ret_context.insert_or_assign("weight3", Serializer::write(instance.weight3));
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
		if(!json_context["bind"].is_null())
		{
			assert(json_context["bind"].is_array());
			Json::array array_bind = 
			json_context["bind"].array_items();
			instance.bind.resize(array_bind.size());
			for(size_t index = 0;index<array_bind.size();++index){
				Serializer::read(array_bind[index],instance.bind[index]);
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
        
        Json::array bind_json;
        for (auto& item : instance.bind){
            bind_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("bind",bind_json);
        
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


	template<>
	RawBone & Serializer::read(const Json & json_context,RawBone & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["name"].is_null())
		{
			Serializer::read(json_context["name"],instance.name);
		}
		if(!json_context["index"].is_null())
		{
			Serializer::read(json_context["index"],instance.index);
		}
		if(!json_context["binding_pose"].is_null())
		{
			Serializer::read(json_context["binding_pose"],instance.binding_pose);
		}
		if(!json_context["tpose_matrix"].is_null())
		{
			Serializer::read(json_context["tpose_matrix"],instance.tpose_matrix);
		}
		if(!json_context["parent_index"].is_null())
		{
			Serializer::read(json_context["parent_index"],instance.parent_index);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const RawBone& instance){
        Json::object  ret_context;
        
        ret_context.insert_or_assign("name", Serializer::write(instance.name));
        ret_context.insert_or_assign("index", Serializer::write(instance.index));
        ret_context.insert_or_assign("binding_pose", Serializer::write(instance.binding_pose));
        ret_context.insert_or_assign("tpose_matrix", Serializer::write(instance.tpose_matrix));
        ret_context.insert_or_assign("parent_index", Serializer::write(instance.parent_index));
        return  Json(ret_context);
    }


	template<>
	SkeletonData & Serializer::read(const Json & json_context,SkeletonData & instance)
	{
		assert(json_context.is_object());
		
		if(!json_context["bones_map"].is_null())
		{
			assert(json_context["bones_map"].is_array());
			Json::array array_bones_map = 
			json_context["bones_map"].array_items();
			instance.bones_map.resize(array_bones_map.size());
			for(size_t index = 0;index<array_bones_map.size();++index){
				Serializer::read(array_bones_map[index],instance.bones_map[index]);
			}
		}
		if(!json_context["is_flat"].is_null())
		{
			Serializer::read(json_context["is_flat"],instance.is_flat);
		}
		if(!json_context["root_index"].is_null())
		{
			Serializer::read(json_context["root_index"],instance.root_index);
		}
		if(!json_context["in_topological_order"].is_null())
		{
			Serializer::read(json_context["in_topological_order"],instance.in_topological_order);
		}
		return instance;
	}

	template<>
    Json Serializer::write(const SkeletonData& instance){
        Json::object  ret_context;
        
        Json::array bones_map_json;
        for (auto& item : instance.bones_map){
            bones_map_json.emplace_back(Serializer::write(item));
        }
        ret_context.insert_or_assign("bones_map",bones_map_json);
        
        ret_context.insert_or_assign("is_flat", Serializer::write(instance.is_flat));
        ret_context.insert_or_assign("root_index", Serializer::write(instance.root_index));
        ret_context.insert_or_assign("in_topological_order", Serializer::write(instance.in_topological_order));
        return  Json(ret_context);
    }


}

