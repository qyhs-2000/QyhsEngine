#pragma once
#include "runtime\function\framework\component\animation\animation.h"

namespace QYHS
{
	class BlendState;
	class AnimationComponentRes;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeBlendStateOperator
			{
			public:
				//class
				//static void getBlendStateBaseClassReflectionInstanceList()
				static const char* getClassName(){return "BlendState";}
				static void* constructorWithJson(const Json & json_context)
				{
					BlendState * ret_instance = new BlendState;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getBlendStateBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(BlendState*)instance_ptr);
				}
				//field
				static void set_blend_anim_skel_map_path(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->blend_anim_skel_map_path = *static_cast<std::vector<std::string>*>(field_value);
				}
				static void* get_blend_anim_skel_map_path(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->blend_anim_skel_map_path);
				}
				static const char* getFieldName_blend_anim_skel_map_path()
				{
					return "blend_anim_skel_map_path";
				}
				static const char* getFieldTypeName_blend_anim_skel_map_path()
				{
					return "std::vector<std::string>";
				}
				static bool isArray_blend_anim_skel_map_path()
				{
					return true;
				}
				static void set_blend_clip_file_path(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->blend_clip_file_path = *static_cast<std::vector<std::string>*>(field_value);
				}
				static void* get_blend_clip_file_path(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->blend_clip_file_path);
				}
				static const char* getFieldName_blend_clip_file_path()
				{
					return "blend_clip_file_path";
				}
				static const char* getFieldTypeName_blend_clip_file_path()
				{
					return "std::vector<std::string>";
				}
				static bool isArray_blend_clip_file_path()
				{
					return true;
				}
				static void set_blend_mask_file_path(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->blend_mask_file_path = *static_cast<std::vector<std::string>*>(field_value);
				}
				static void* get_blend_mask_file_path(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->blend_mask_file_path);
				}
				static const char* getFieldName_blend_mask_file_path()
				{
					return "blend_mask_file_path";
				}
				static const char* getFieldTypeName_blend_mask_file_path()
				{
					return "std::vector<std::string>";
				}
				static bool isArray_blend_mask_file_path()
				{
					return true;
				}
				static void set_blend_clip_file_length(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->blend_clip_file_length = *static_cast<std::vector<float>*>(field_value);
				}
				static void* get_blend_clip_file_length(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->blend_clip_file_length);
				}
				static const char* getFieldName_blend_clip_file_length()
				{
					return "blend_clip_file_length";
				}
				static const char* getFieldTypeName_blend_clip_file_length()
				{
					return "std::vector<float>";
				}
				static bool isArray_blend_clip_file_length()
				{
					return true;
				}
				static void set_blend_ratio(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->blend_ratio = *static_cast<std::vector<float>*>(field_value);
				}
				static void* get_blend_ratio(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->blend_ratio);
				}
				static const char* getFieldName_blend_ratio()
				{
					return "blend_ratio";
				}
				static const char* getFieldTypeName_blend_ratio()
				{
					return "std::vector<float>";
				}
				static bool isArray_blend_ratio()
				{
					return true;
				}
				static void set_blend_weight(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->blend_weight = *static_cast<std::vector<float>*>(field_value);
				}
				static void* get_blend_weight(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->blend_weight);
				}
				static const char* getFieldName_blend_weight()
				{
					return "blend_weight";
				}
				static const char* getFieldTypeName_blend_weight()
				{
					return "std::vector<float>";
				}
				static bool isArray_blend_weight()
				{
					return true;
				}
				static void set_clip_count(void * instance,void * field_value)
				{
					static_cast<BlendState*>(instance)->clip_count = *static_cast<int*>(field_value);
				}
				static void* get_clip_count(void * instance)
				{
					return &(static_cast<BlendState*>(instance)->clip_count);
				}
				static const char* getFieldName_clip_count()
				{
					return "clip_count";
				}
				static const char* getFieldTypeName_clip_count()
				{
					return "int";
				}
				static bool isArray_clip_count()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLfloatROperatorMacro
		#define ArraystdSSvectorLfloatROperatorMacro
		class ArraystdSSvectorLfloatROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<float>";}
			static const char* getElementTypeName(){return "float";}
			static int getSize(void * instance){return (static_cast<std::vector<float>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<float>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<float>*>(instance))[index] = *(static_cast<float*>(value));
			}
		};
		#endif
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
			class TypeAnimationComponentResOperator
			{
			public:
				//class
				//static void getAnimationComponentResBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationComponentRes";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationComponentRes * ret_instance = new AnimationComponentRes;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationComponentResBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationComponentRes*)instance_ptr);
				}
				//field
				static void set_skeleton_file_path(void * instance,void * field_value)
				{
					static_cast<AnimationComponentRes*>(instance)->skeleton_file_path = *static_cast<std::string*>(field_value);
				}
				static void* get_skeleton_file_path(void * instance)
				{
					return &(static_cast<AnimationComponentRes*>(instance)->skeleton_file_path);
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
				static void set_frame_position(void * instance,void * field_value)
				{
					static_cast<AnimationComponentRes*>(instance)->frame_position = *static_cast<float*>(field_value);
				}
				static void* get_frame_position(void * instance)
				{
					return &(static_cast<AnimationComponentRes*>(instance)->frame_position);
				}
				static const char* getFieldName_frame_position()
				{
					return "frame_position";
				}
				static const char* getFieldTypeName_frame_position()
				{
					return "float";
				}
				static bool isArray_frame_position()
				{
					return false;
				}
				static void set_blend_state(void * instance,void * field_value)
				{
					static_cast<AnimationComponentRes*>(instance)->blend_state = *static_cast<BlendState*>(field_value);
				}
				static void* get_blend_state(void * instance)
				{
					return &(static_cast<AnimationComponentRes*>(instance)->blend_state);
				}
				static const char* getFieldName_blend_state()
				{
					return "blend_state";
				}
				static const char* getFieldTypeName_blend_state()
				{
					return "BlendState";
				}
				static bool isArray_blend_state()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_BlendState()
		{
			FieldFunctionTuple * f_field_function_tuple_blend_anim_skel_map_path = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_blend_anim_skel_map_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_blend_anim_skel_map_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_blend_anim_skel_map_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_blend_anim_skel_map_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_blend_anim_skel_map_path
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_blend_anim_skel_map_path);
			FieldFunctionTuple * f_field_function_tuple_blend_clip_file_path = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_blend_clip_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_blend_clip_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_blend_clip_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_blend_clip_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_blend_clip_file_path
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_blend_clip_file_path);
			FieldFunctionTuple * f_field_function_tuple_blend_mask_file_path = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_blend_mask_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_blend_mask_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_blend_mask_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_blend_mask_file_path,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_blend_mask_file_path
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_blend_mask_file_path);
			FieldFunctionTuple * f_field_function_tuple_blend_clip_file_length = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_blend_clip_file_length,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_blend_clip_file_length,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_blend_clip_file_length,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_blend_clip_file_length,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_blend_clip_file_length
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_blend_clip_file_length);
			FieldFunctionTuple * f_field_function_tuple_blend_ratio = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_blend_ratio,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_blend_ratio,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_blend_ratio,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_blend_ratio,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_blend_ratio
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_blend_ratio);
			FieldFunctionTuple * f_field_function_tuple_blend_weight = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_blend_weight,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_blend_weight,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_blend_weight,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_blend_weight,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_blend_weight
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_blend_weight);
			FieldFunctionTuple * f_field_function_tuple_clip_count = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBlendStateOperator::set_clip_count,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::get_clip_count,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldName_clip_count,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::getFieldTypeName_clip_count,
			&TypeFieldReflectionOperator::TypeBlendStateOperator::isArray_clip_count
			);
			REGISTER_TO_FIELD_MAP("BlendState",f_field_function_tuple_clip_count);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLfloatR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLfloatR",array_function_tuple_stdSSvectorLfloatR);
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLstdSSstringR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLstdSSstringROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLstdSSstringR",array_function_tuple_stdSSvectorLstdSSstringR);
		ClassFunctionTuple * f_class_function_tuple_BlendState = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeBlendStateOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeBlendStateOperator::getBlendStateBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeBlendStateOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("BlendState",f_class_function_tuple_BlendState);
		}
		void TypeWrapperRegister_AnimationComponentRes()
		{
			FieldFunctionTuple * f_field_function_tuple_skeleton_file_path = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::set_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::get_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getFieldName_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getFieldTypeName_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::isArray_skeleton_file_path
			);
			REGISTER_TO_FIELD_MAP("AnimationComponentRes",f_field_function_tuple_skeleton_file_path);
			FieldFunctionTuple * f_field_function_tuple_frame_position = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::set_frame_position,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::get_frame_position,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getFieldName_frame_position,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getFieldTypeName_frame_position,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::isArray_frame_position
			);
			REGISTER_TO_FIELD_MAP("AnimationComponentRes",f_field_function_tuple_frame_position);
			FieldFunctionTuple * f_field_function_tuple_blend_state = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::set_blend_state,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::get_blend_state,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getFieldName_blend_state,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getFieldTypeName_blend_state,
			&TypeFieldReflectionOperator::TypeAnimationComponentResOperator::isArray_blend_state
			);
			REGISTER_TO_FIELD_MAP("AnimationComponentRes",f_field_function_tuple_blend_state);
		
		
		ClassFunctionTuple * f_class_function_tuple_AnimationComponentRes = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationComponentResOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationComponentResOperator::getAnimationComponentResBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationComponentResOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationComponentRes",f_class_function_tuple_AnimationComponentRes);
		}
		namespace TypeWrappersRegister
		{
			void BlendState(){TypeWrapperRegister_BlendState();}
			void AnimationComponentRes(){TypeWrapperRegister_AnimationComponentRes();}
		}
		
	}
}

