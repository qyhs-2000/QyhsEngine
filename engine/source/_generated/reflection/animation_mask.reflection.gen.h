#pragma once
#include "runtime\function\framework\component\animation\animation_mask.h"

namespace QYHS
{
	class BoneBlendMask;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeBoneBlendMaskOperator
			{
			public:
				//class
				//static void getBoneBlendMaskBaseClassReflectionInstanceList()
				static const char* getClassName(){return "BoneBlendMask";}
				static void* constructorWithJson(const Json & json_context)
				{
					BoneBlendMask * ret_instance = new BoneBlendMask;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getBoneBlendMaskBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(BoneBlendMask*)instance_ptr);
				}
				//field
				static void set_skeleton_file_path(void * instance,void * field_value)
				{
					static_cast<BoneBlendMask*>(instance)->skeleton_file_path = *static_cast<std::string*>(field_value);
				}
				static void* get_skeleton_file_path(void * instance)
				{
					return &(static_cast<BoneBlendMask*>(instance)->skeleton_file_path);
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
				static void set_enabled(void * instance,void * field_value)
				{
					static_cast<BoneBlendMask*>(instance)->enabled = *static_cast<std::vector<float>*>(field_value);
				}
				static void* get_enabled(void * instance)
				{
					return &(static_cast<BoneBlendMask*>(instance)->enabled);
				}
				static const char* getFieldName_enabled()
				{
					return "enabled";
				}
				static const char* getFieldTypeName_enabled()
				{
					return "std::vector<float>";
				}
				static bool isArray_enabled()
				{
					return true;
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
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_BoneBlendMask()
		{
			FieldFunctionTuple * f_field_function_tuple_skeleton_file_path = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::set_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::get_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getFieldName_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getFieldTypeName_skeleton_file_path,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::isArray_skeleton_file_path
			);
			REGISTER_TO_FIELD_MAP("BoneBlendMask",f_field_function_tuple_skeleton_file_path);
			FieldFunctionTuple * f_field_function_tuple_enabled = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::set_enabled,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::get_enabled,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getClassName,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getFieldName_enabled,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getFieldTypeName_enabled,
			&TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::isArray_enabled
			);
			REGISTER_TO_FIELD_MAP("BoneBlendMask",f_field_function_tuple_enabled);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLfloatR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLfloatROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLfloatR",array_function_tuple_stdSSvectorLfloatR);
		ClassFunctionTuple * f_class_function_tuple_BoneBlendMask = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::getBoneBlendMaskBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeBoneBlendMaskOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("BoneBlendMask",f_class_function_tuple_BoneBlendMask);
		}
		namespace TypeWrappersRegister
		{
			void BoneBlendMask(){TypeWrapperRegister_BoneBlendMask();}
		}
		
	}
}

