#pragma once
#include "runtime\resource\type\data\skeleton_data.h"

namespace QYHS
{
	class RawBone;
	class SkeletonData;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeRawBoneOperator
			{
			public:
				//class
				//static void getRawBoneBaseClassReflectionInstanceList()
				static const char* getClassName(){return "RawBone";}
				static void* constructorWithJson(const Json & json_context)
				{
					RawBone * ret_instance = new RawBone;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getRawBoneBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(RawBone*)instance_ptr);
				}
				//field
				static void set_name(void * instance,void * field_value)
				{
					static_cast<RawBone*>(instance)->name = *static_cast<std::string*>(field_value);
				}
				static void* get_name(void * instance)
				{
					return &(static_cast<RawBone*>(instance)->name);
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
				static void set_index(void * instance,void * field_value)
				{
					static_cast<RawBone*>(instance)->index = *static_cast<int*>(field_value);
				}
				static void* get_index(void * instance)
				{
					return &(static_cast<RawBone*>(instance)->index);
				}
				static const char* getFieldName_index()
				{
					return "index";
				}
				static const char* getFieldTypeName_index()
				{
					return "int";
				}
				static bool isArray_index()
				{
					return false;
				}
				static void set_binding_pose(void * instance,void * field_value)
				{
					static_cast<RawBone*>(instance)->binding_pose = *static_cast<Transform*>(field_value);
				}
				static void* get_binding_pose(void * instance)
				{
					return &(static_cast<RawBone*>(instance)->binding_pose);
				}
				static const char* getFieldName_binding_pose()
				{
					return "binding_pose";
				}
				static const char* getFieldTypeName_binding_pose()
				{
					return "Transform";
				}
				static bool isArray_binding_pose()
				{
					return false;
				}
				static void set_tpose_matrix(void * instance,void * field_value)
				{
					static_cast<RawBone*>(instance)->tpose_matrix = *static_cast<Matrix4x4_*>(field_value);
				}
				static void* get_tpose_matrix(void * instance)
				{
					return &(static_cast<RawBone*>(instance)->tpose_matrix);
				}
				static const char* getFieldName_tpose_matrix()
				{
					return "tpose_matrix";
				}
				static const char* getFieldTypeName_tpose_matrix()
				{
					return "Matrix4x4_";
				}
				static bool isArray_tpose_matrix()
				{
					return false;
				}
				static void set_parent_index(void * instance,void * field_value)
				{
					static_cast<RawBone*>(instance)->parent_index = *static_cast<int*>(field_value);
				}
				static void* get_parent_index(void * instance)
				{
					return &(static_cast<RawBone*>(instance)->parent_index);
				}
				static const char* getFieldName_parent_index()
				{
					return "parent_index";
				}
				static const char* getFieldTypeName_parent_index()
				{
					return "int";
				}
				static bool isArray_parent_index()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeSkeletonDataOperator
			{
			public:
				//class
				//static void getSkeletonDataBaseClassReflectionInstanceList()
				static const char* getClassName(){return "SkeletonData";}
				static void* constructorWithJson(const Json & json_context)
				{
					SkeletonData * ret_instance = new SkeletonData;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getSkeletonDataBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(SkeletonData*)instance_ptr);
				}
				//field
				static void set_bones_map(void * instance,void * field_value)
				{
					static_cast<SkeletonData*>(instance)->bones_map = *static_cast<std::vector<RawBone>*>(field_value);
				}
				static void* get_bones_map(void * instance)
				{
					return &(static_cast<SkeletonData*>(instance)->bones_map);
				}
				static const char* getFieldName_bones_map()
				{
					return "bones_map";
				}
				static const char* getFieldTypeName_bones_map()
				{
					return "std::vector<RawBone>";
				}
				static bool isArray_bones_map()
				{
					return true;
				}
				static void set_is_flat(void * instance,void * field_value)
				{
					static_cast<SkeletonData*>(instance)->is_flat = *static_cast<bool*>(field_value);
				}
				static void* get_is_flat(void * instance)
				{
					return &(static_cast<SkeletonData*>(instance)->is_flat);
				}
				static const char* getFieldName_is_flat()
				{
					return "is_flat";
				}
				static const char* getFieldTypeName_is_flat()
				{
					return "bool";
				}
				static bool isArray_is_flat()
				{
					return false;
				}
				static void set_root_index(void * instance,void * field_value)
				{
					static_cast<SkeletonData*>(instance)->root_index = *static_cast<int*>(field_value);
				}
				static void* get_root_index(void * instance)
				{
					return &(static_cast<SkeletonData*>(instance)->root_index);
				}
				static const char* getFieldName_root_index()
				{
					return "root_index";
				}
				static const char* getFieldTypeName_root_index()
				{
					return "int";
				}
				static bool isArray_root_index()
				{
					return false;
				}
				static void set_in_topological_order(void * instance,void * field_value)
				{
					static_cast<SkeletonData*>(instance)->in_topological_order = *static_cast<bool*>(field_value);
				}
				static void* get_in_topological_order(void * instance)
				{
					return &(static_cast<SkeletonData*>(instance)->in_topological_order);
				}
				static const char* getFieldName_in_topological_order()
				{
					return "in_topological_order";
				}
				static const char* getFieldTypeName_in_topological_order()
				{
					return "bool";
				}
				static bool isArray_in_topological_order()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLRawBoneROperatorMacro
		#define ArraystdSSvectorLRawBoneROperatorMacro
		class ArraystdSSvectorLRawBoneROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<RawBone>";}
			static const char* getElementTypeName(){return "RawBone";}
			static int getSize(void * instance){return (static_cast<std::vector<RawBone>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<RawBone>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<RawBone>*>(instance))[index] = *(static_cast<RawBone*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_RawBone()
		{
			FieldFunctionTuple * f_field_function_tuple_name = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeRawBoneOperator::set_name,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::get_name,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getClassName,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldName_name,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldTypeName_name,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::isArray_name
			);
			REGISTER_TO_FIELD_MAP("RawBone",f_field_function_tuple_name);
			FieldFunctionTuple * f_field_function_tuple_index = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeRawBoneOperator::set_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::get_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getClassName,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldName_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldTypeName_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::isArray_index
			);
			REGISTER_TO_FIELD_MAP("RawBone",f_field_function_tuple_index);
			FieldFunctionTuple * f_field_function_tuple_binding_pose = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeRawBoneOperator::set_binding_pose,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::get_binding_pose,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getClassName,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldName_binding_pose,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldTypeName_binding_pose,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::isArray_binding_pose
			);
			REGISTER_TO_FIELD_MAP("RawBone",f_field_function_tuple_binding_pose);
			FieldFunctionTuple * f_field_function_tuple_tpose_matrix = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeRawBoneOperator::set_tpose_matrix,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::get_tpose_matrix,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getClassName,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldName_tpose_matrix,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldTypeName_tpose_matrix,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::isArray_tpose_matrix
			);
			REGISTER_TO_FIELD_MAP("RawBone",f_field_function_tuple_tpose_matrix);
			FieldFunctionTuple * f_field_function_tuple_parent_index = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeRawBoneOperator::set_parent_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::get_parent_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getClassName,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldName_parent_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::getFieldTypeName_parent_index,
			&TypeFieldReflectionOperator::TypeRawBoneOperator::isArray_parent_index
			);
			REGISTER_TO_FIELD_MAP("RawBone",f_field_function_tuple_parent_index);
		
		
		ClassFunctionTuple * f_class_function_tuple_RawBone = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeRawBoneOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeRawBoneOperator::getRawBoneBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeRawBoneOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("RawBone",f_class_function_tuple_RawBone);
		}
		void TypeWrapperRegister_SkeletonData()
		{
			FieldFunctionTuple * f_field_function_tuple_bones_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::set_bones_map,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::get_bones_map,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldName_bones_map,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldTypeName_bones_map,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::isArray_bones_map
			);
			REGISTER_TO_FIELD_MAP("SkeletonData",f_field_function_tuple_bones_map);
			FieldFunctionTuple * f_field_function_tuple_is_flat = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::set_is_flat,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::get_is_flat,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldName_is_flat,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldTypeName_is_flat,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::isArray_is_flat
			);
			REGISTER_TO_FIELD_MAP("SkeletonData",f_field_function_tuple_is_flat);
			FieldFunctionTuple * f_field_function_tuple_root_index = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::set_root_index,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::get_root_index,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldName_root_index,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldTypeName_root_index,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::isArray_root_index
			);
			REGISTER_TO_FIELD_MAP("SkeletonData",f_field_function_tuple_root_index);
			FieldFunctionTuple * f_field_function_tuple_in_topological_order = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::set_in_topological_order,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::get_in_topological_order,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldName_in_topological_order,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::getFieldTypeName_in_topological_order,
			&TypeFieldReflectionOperator::TypeSkeletonDataOperator::isArray_in_topological_order
			);
			REGISTER_TO_FIELD_MAP("SkeletonData",f_field_function_tuple_in_topological_order);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLRawBoneR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLRawBoneROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLRawBoneROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLRawBoneROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLRawBoneROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLRawBoneROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLRawBoneR",array_function_tuple_stdSSvectorLRawBoneR);
		ClassFunctionTuple * f_class_function_tuple_SkeletonData = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeSkeletonDataOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeSkeletonDataOperator::getSkeletonDataBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeSkeletonDataOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("SkeletonData",f_class_function_tuple_SkeletonData);
		}
		namespace TypeWrappersRegister
		{
			void RawBone(){TypeWrapperRegister_RawBone();}
			void SkeletonData(){TypeWrapperRegister_SkeletonData();}
		}
		
	}
}

