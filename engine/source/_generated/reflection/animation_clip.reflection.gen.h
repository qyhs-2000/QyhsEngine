#pragma once
#include "runtime\function\framework\component\animation\animation_clip.h"

namespace qyhs
{
	class AnimationNodeMap;
	class AnimationChannel;
	class AnimationClip;
	class AnimationAsset;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationNodeMapOperator
			{
			public:
				//class
				//static void getAnimationNodeMapBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationNodeMap";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationNodeMap * ret_instance = new AnimationNodeMap;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationNodeMapBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationNodeMap*)instance_ptr);
				}
				//field
				static void set_convert(void * instance,void * field_value)
				{
					static_cast<AnimationNodeMap*>(instance)->convert = *static_cast<std::vector<std::string>*>(field_value);
				}
				static void* get_convert(void * instance)
				{
					return &(static_cast<AnimationNodeMap*>(instance)->convert);
				}
				static const char* getFieldName_convert()
				{
					return "convert";
				}
				static const char* getFieldTypeName_convert()
				{
					return "std::vector<std::string>";
				}
				static bool isArray_convert()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLstdSSstringROperatorMacro
		#define ArraystdSSvectorLstdSSstringROperatorMacro
		class ArraystdSSvectorLstdSSstringROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<std::string>";}
			static const char* getElementTypeName(){return "std::string";}
			static int getSize(void * instance){return (static_cast<std::vector<std::string>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<std::string>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<std::string>*>(instance))[index] = *(static_cast<std::string*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationChannelOperator
			{
			public:
				//class
				//static void getAnimationChannelBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationChannel";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationChannel * ret_instance = new AnimationChannel;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationChannelBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationChannel*)instance_ptr);
				}
				//field
				static void set_name(void * instance,void * field_value)
				{
					static_cast<AnimationChannel*>(instance)->name = *static_cast<std::string*>(field_value);
				}
				static void* get_name(void * instance)
				{
					return &(static_cast<AnimationChannel*>(instance)->name);
				}
				static const char* getFieldName_name()
				{
					return "name";
				}
				static const char* getFieldTypeName_name()
				{
					return "std::string";
				}
				static bool isArray_name()
				{
					return false;
				}
				static void set_position_keys(void * instance,void * field_value)
				{
					static_cast<AnimationChannel*>(instance)->position_keys = *static_cast<std::vector<Vector3>*>(field_value);
				}
				static void* get_position_keys(void * instance)
				{
					return &(static_cast<AnimationChannel*>(instance)->position_keys);
				}
				static const char* getFieldName_position_keys()
				{
					return "position_keys";
				}
				static const char* getFieldTypeName_position_keys()
				{
					return "std::vector<Vector3>";
				}
				static bool isArray_position_keys()
				{
					return true;
				}
				static void set_rotation_keys(void * instance,void * field_value)
				{
					static_cast<AnimationChannel*>(instance)->rotation_keys = *static_cast<std::vector<Quaternion>*>(field_value);
				}
				static void* get_rotation_keys(void * instance)
				{
					return &(static_cast<AnimationChannel*>(instance)->rotation_keys);
				}
				static const char* getFieldName_rotation_keys()
				{
					return "rotation_keys";
				}
				static const char* getFieldTypeName_rotation_keys()
				{
					return "std::vector<Quaternion>";
				}
				static bool isArray_rotation_keys()
				{
					return true;
				}
				static void set_scaling_keys(void * instance,void * field_value)
				{
					static_cast<AnimationChannel*>(instance)->scaling_keys = *static_cast<std::vector<Vector3>*>(field_value);
				}
				static void* get_scaling_keys(void * instance)
				{
					return &(static_cast<AnimationChannel*>(instance)->scaling_keys);
				}
				static const char* getFieldName_scaling_keys()
				{
					return "scaling_keys";
				}
				static const char* getFieldTypeName_scaling_keys()
				{
					return "std::vector<Vector3>";
				}
				static bool isArray_scaling_keys()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLQuaternionROperatorMacro
		#define ArraystdSSvectorLQuaternionROperatorMacro
		class ArraystdSSvectorLQuaternionROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<Quaternion>";}
			static const char* getElementTypeName(){return "Quaternion";}
			static int getSize(void * instance){return (static_cast<std::vector<Quaternion>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<Quaternion>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<Quaternion>*>(instance))[index] = *(static_cast<Quaternion*>(value));
			}
		};
		#endif
		#ifndef ArraystdSSvectorLVector3ROperatorMacro
		#define ArraystdSSvectorLVector3ROperatorMacro
		class ArraystdSSvectorLVector3ROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<Vector3>";}
			static const char* getElementTypeName(){return "Vector3";}
			static int getSize(void * instance){return (static_cast<std::vector<Vector3>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<Vector3>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<Vector3>*>(instance))[index] = *(static_cast<Vector3*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationClipOperator
			{
			public:
				//class
				//static void getAnimationClipBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationClip";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationClip * ret_instance = new AnimationClip;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationClipBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationClip*)instance_ptr);
				}
				//field
				static void set_total_frame(void * instance,void * field_value)
				{
					static_cast<AnimationClip*>(instance)->total_frame = *static_cast<int*>(field_value);
				}
				static void* get_total_frame(void * instance)
				{
					return &(static_cast<AnimationClip*>(instance)->total_frame);
				}
				static const char* getFieldName_total_frame()
				{
					return "total_frame";
				}
				static const char* getFieldTypeName_total_frame()
				{
					return "int";
				}
				static bool isArray_total_frame()
				{
					return false;
				}
				static void set_node_count(void * instance,void * field_value)
				{
					static_cast<AnimationClip*>(instance)->node_count = *static_cast<int*>(field_value);
				}
				static void* get_node_count(void * instance)
				{
					return &(static_cast<AnimationClip*>(instance)->node_count);
				}
				static const char* getFieldName_node_count()
				{
					return "node_count";
				}
				static const char* getFieldTypeName_node_count()
				{
					return "int";
				}
				static bool isArray_node_count()
				{
					return false;
				}
				static void set_node_channels(void * instance,void * field_value)
				{
					static_cast<AnimationClip*>(instance)->node_channels = *static_cast<std::vector<AnimationChannel>*>(field_value);
				}
				static void* get_node_channels(void * instance)
				{
					return &(static_cast<AnimationClip*>(instance)->node_channels);
				}
				static const char* getFieldName_node_channels()
				{
					return "node_channels";
				}
				static const char* getFieldTypeName_node_channels()
				{
					return "std::vector<AnimationChannel>";
				}
				static bool isArray_node_channels()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLAnimationChannelROperatorMacro
		#define ArraystdSSvectorLAnimationChannelROperatorMacro
		class ArraystdSSvectorLAnimationChannelROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<AnimationChannel>";}
			static const char* getElementTypeName(){return "AnimationChannel";}
			static int getSize(void * instance){return (static_cast<std::vector<AnimationChannel>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<AnimationChannel>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<AnimationChannel>*>(instance))[index] = *(static_cast<AnimationChannel*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationAssetOperator
			{
			public:
				//class
				//static void getAnimationAssetBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationAsset";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationAsset * ret_instance = new AnimationAsset;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationAssetBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationAsset*)instance_ptr);
				}
				//field
				static void set_node_map(void * instance,void * field_value)
				{
					static_cast<AnimationAsset*>(instance)->node_map = *static_cast<AnimationNodeMap*>(field_value);
				}
				static void* get_node_map(void * instance)
				{
					return &(static_cast<AnimationAsset*>(instance)->node_map);
				}
				static const char* getFieldName_node_map()
				{
					return "node_map";
				}
				static const char* getFieldTypeName_node_map()
				{
					return "AnimationNodeMap";
				}
				static bool isArray_node_map()
				{
					return false;
				}
				static void set_clip_data(void * instance,void * field_value)
				{
					static_cast<AnimationAsset*>(instance)->clip_data = *static_cast<AnimationClip*>(field_value);
				}
				static void* get_clip_data(void * instance)
				{
					return &(static_cast<AnimationAsset*>(instance)->clip_data);
				}
				static const char* getFieldName_clip_data()
				{
					return "clip_data";
				}
				static const char* getFieldTypeName_clip_data()
				{
					return "AnimationClip";
				}
				static bool isArray_clip_data()
				{
					return false;
				}
				static void set_skeleton_file_path(void * instance,void * field_value)
				{
					static_cast<AnimationAsset*>(instance)->skeleton_file_path = *static_cast<std::string*>(field_value);
				}
				static void* get_skeleton_file_path(void * instance)
				{
					return &(static_cast<AnimationAsset*>(instance)->skeleton_file_path);
				}
				static const char* getFieldName_skeleton_file_path()
				{
					return "skeleton_file_path";
				}
				static const char* getFieldTypeName_skeleton_file_path()
				{
					return "std::string";
				}
				static bool isArray_skeleton_file_path()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_AnimationNodeMap()
		{
			FieldFunctionTuple * f_field_function_tuple_convert = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::set_convert,
			&TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::get_convert,
			&TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::getFieldName_convert,
			&TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::getFieldTypeName_convert,
			&TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::isArray_convert
			);
			REGISTER_TO_FIELD_MAP("AnimationNodeMap",f_field_function_tuple_convert);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLstdSSstringR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLstdSSstringR",array_function_tuple_stdSSvectorLstdSSstringR);
		ClassFunctionTuple * f_class_function_tuple_AnimationNodeMap = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::getAnimationNodeMapBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationNodeMapOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationNodeMap",f_class_function_tuple_AnimationNodeMap);
		}
		void TypeWrapperRegister_AnimationChannel()
		{
			FieldFunctionTuple * f_field_function_tuple_name = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::set_name,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::get_name,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldName_name,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldTypeName_name,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::isArray_name
			);
			REGISTER_TO_FIELD_MAP("AnimationChannel",f_field_function_tuple_name);
			FieldFunctionTuple * f_field_function_tuple_position_keys = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::set_position_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::get_position_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldName_position_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldTypeName_position_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::isArray_position_keys
			);
			REGISTER_TO_FIELD_MAP("AnimationChannel",f_field_function_tuple_position_keys);
			FieldFunctionTuple * f_field_function_tuple_rotation_keys = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::set_rotation_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::get_rotation_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldName_rotation_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldTypeName_rotation_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::isArray_rotation_keys
			);
			REGISTER_TO_FIELD_MAP("AnimationChannel",f_field_function_tuple_rotation_keys);
			FieldFunctionTuple * f_field_function_tuple_scaling_keys = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::set_scaling_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::get_scaling_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldName_scaling_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::getFieldTypeName_scaling_keys,
			&TypeFieldReflectionOperator::TypeAnimationChannelOperator::isArray_scaling_keys
			);
			REGISTER_TO_FIELD_MAP("AnimationChannel",f_field_function_tuple_scaling_keys);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLQuaternionR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLQuaternionROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLQuaternionROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLQuaternionROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLQuaternionROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLQuaternionROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLQuaternionR",array_function_tuple_stdSSvectorLQuaternionR);
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLVector3R = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLVector3ROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLVector3ROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLVector3ROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLVector3ROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLVector3ROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLVector3R",array_function_tuple_stdSSvectorLVector3R);
		ClassFunctionTuple * f_class_function_tuple_AnimationChannel = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationChannelOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationChannelOperator::getAnimationChannelBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationChannelOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationChannel",f_class_function_tuple_AnimationChannel);
		}
		void TypeWrapperRegister_AnimationClip()
		{
			FieldFunctionTuple * f_field_function_tuple_total_frame = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::set_total_frame,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::get_total_frame,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getFieldName_total_frame,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getFieldTypeName_total_frame,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::isArray_total_frame
			);
			REGISTER_TO_FIELD_MAP("AnimationClip",f_field_function_tuple_total_frame);
			FieldFunctionTuple * f_field_function_tuple_node_count = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::set_node_count,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::get_node_count,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getFieldName_node_count,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getFieldTypeName_node_count,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::isArray_node_count
			);
			REGISTER_TO_FIELD_MAP("AnimationClip",f_field_function_tuple_node_count);
			FieldFunctionTuple * f_field_function_tuple_node_channels = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::set_node_channels,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::get_node_channels,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getFieldName_node_channels,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::getFieldTypeName_node_channels,
			&TypeFieldReflectionOperator::TypeAnimationClipOperator::isArray_node_channels
			);
			REGISTER_TO_FIELD_MAP("AnimationClip",f_field_function_tuple_node_channels);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLAnimationChannelR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLAnimationChannelROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLAnimationChannelROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLAnimationChannelROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLAnimationChannelROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLAnimationChannelROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLAnimationChannelR",array_function_tuple_stdSSvectorLAnimationChannelR);
		ClassFunctionTuple * f_class_function_tuple_AnimationClip = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationClipOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationClipOperator::getAnimationClipBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationClipOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationClip",f_class_function_tuple_AnimationClip);
		}
		void TypeWrapperRegister_AnimationAsset()
		{
			FieldFunctionTuple * f_field_function_tuple_node_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::set_node_map,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::get_node_map,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getFieldName_node_map,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getFieldTypeName_node_map,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::isArray_node_map
			);
			REGISTER_TO_FIELD_MAP("AnimationAsset",f_field_function_tuple_node_map);
			FieldFunctionTuple * f_field_function_tuple_clip_data = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::set_clip_data,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::get_clip_data,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getFieldName_clip_data,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getFieldTypeName_clip_data,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::isArray_clip_data
			);
			REGISTER_TO_FIELD_MAP("AnimationAsset",f_field_function_tuple_clip_data);
			FieldFunctionTuple * f_field_function_tuple_skeleton_file_path = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::set_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::get_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getFieldName_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::getFieldTypeName_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationAssetOperator::isArray_skeleton_file_path
			);
			REGISTER_TO_FIELD_MAP("AnimationAsset",f_field_function_tuple_skeleton_file_path);
		
		
		ClassFunctionTuple * f_class_function_tuple_AnimationAsset = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationAssetOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationAssetOperator::getAnimationAssetBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationAssetOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationAsset",f_class_function_tuple_AnimationAsset);
		}
		namespace TypeWrappersRegister
		{
			void AnimationNodeMap(){TypeWrapperRegister_AnimationNodeMap();}
			void AnimationChannel(){TypeWrapperRegister_AnimationChannel();}
			void AnimationClip(){TypeWrapperRegister_AnimationClip();}
			void AnimationAsset(){TypeWrapperRegister_AnimationAsset();}
		}
		
	}
}

