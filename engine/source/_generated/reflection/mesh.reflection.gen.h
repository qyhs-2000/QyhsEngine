#pragma once
#include "runtime\resource\type\component\mesh\mesh.h"

namespace QYHS
{
	class SubMeshRes;
	class MeshComponentRes;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeSubMeshResOperator
			{
			public:
				//class
				//static void getSubMeshResBaseClassReflectionInstanceList()
				static const char* getClassName(){return "SubMeshRes";}
				static void* constructorWithJson(const Json & json_context)
				{
					SubMeshRes * ret_instance = new SubMeshRes;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getSubMeshResBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(SubMeshRes*)instance_ptr);
				}
				//field
				static void set_m_material(void * instance,void * field_value)
				{
					static_cast<SubMeshRes*>(instance)->m_material = *static_cast<std::string*>(field_value);
				}
				static void* get_m_material(void * instance)
				{
					return &(static_cast<SubMeshRes*>(instance)->m_material);
				}
				static const char* getFieldName_m_material()
				{
					return "m_material";
				}
				static const char* getFieldTypeName_m_material()
				{
					return "std::string";
				}
				static bool isArray_m_material()
				{
					return false;
				}
				static void set_m_obj_file_ref(void * instance,void * field_value)
				{
					static_cast<SubMeshRes*>(instance)->m_obj_file_ref = *static_cast<std::string*>(field_value);
				}
				static void* get_m_obj_file_ref(void * instance)
				{
					return &(static_cast<SubMeshRes*>(instance)->m_obj_file_ref);
				}
				static const char* getFieldName_m_obj_file_ref()
				{
					return "m_obj_file_ref";
				}
				static const char* getFieldTypeName_m_obj_file_ref()
				{
					return "std::string";
				}
				static bool isArray_m_obj_file_ref()
				{
					return false;
				}
				static void set_m_transform(void * instance,void * field_value)
				{
					static_cast<SubMeshRes*>(instance)->m_transform = *static_cast<Transform*>(field_value);
				}
				static void* get_m_transform(void * instance)
				{
					return &(static_cast<SubMeshRes*>(instance)->m_transform);
				}
				static const char* getFieldName_m_transform()
				{
					return "m_transform";
				}
				static const char* getFieldTypeName_m_transform()
				{
					return "Transform";
				}
				static bool isArray_m_transform()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeMeshComponentResOperator
			{
			public:
				//class
				//static void getMeshComponentResBaseClassReflectionInstanceList()
				static const char* getClassName(){return "MeshComponentRes";}
				static void* constructorWithJson(const Json & json_context)
				{
					MeshComponentRes * ret_instance = new MeshComponentRes;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getMeshComponentResBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(MeshComponentRes*)instance_ptr);
				}
				//field
				static void set_m_sub_meshes(void * instance,void * field_value)
				{
					static_cast<MeshComponentRes*>(instance)->m_sub_meshes = *static_cast<std::vector<SubMeshRes>*>(field_value);
				}
				static void* get_m_sub_meshes(void * instance)
				{
					return &(static_cast<MeshComponentRes*>(instance)->m_sub_meshes);
				}
				static const char* getFieldName_m_sub_meshes()
				{
					return "m_sub_meshes";
				}
				static const char* getFieldTypeName_m_sub_meshes()
				{
					return "std::vector<SubMeshRes>";
				}
				static bool isArray_m_sub_meshes()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLSubMeshResROperatorMacro
		#define ArraystdSSvectorLSubMeshResROperatorMacro
		class ArraystdSSvectorLSubMeshResROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<SubMeshRes>";}
			static const char* getElementTypeName(){return "SubMeshRes";}
			static int getSize(void * instance){return (static_cast<std::vector<SubMeshRes>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<SubMeshRes>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<SubMeshRes>*>(instance))[index] = *(static_cast<SubMeshRes*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_SubMeshRes()
		{
			FieldFunctionTuple * f_field_function_tuple_m_material = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::set_m_material,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::get_m_material,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getFieldName_m_material,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getFieldTypeName_m_material,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::isArray_m_material
			);
			REGISTER_TO_FIELD_MAP("SubMeshRes",f_field_function_tuple_m_material);
			FieldFunctionTuple * f_field_function_tuple_m_obj_file_ref = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::set_m_obj_file_ref,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::get_m_obj_file_ref,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getFieldName_m_obj_file_ref,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getFieldTypeName_m_obj_file_ref,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::isArray_m_obj_file_ref
			);
			REGISTER_TO_FIELD_MAP("SubMeshRes",f_field_function_tuple_m_obj_file_ref);
			FieldFunctionTuple * f_field_function_tuple_m_transform = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::set_m_transform,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::get_m_transform,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getFieldName_m_transform,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::getFieldTypeName_m_transform,
			&TypeFieldReflectionOperator::TypeSubMeshResOperator::isArray_m_transform
			);
			REGISTER_TO_FIELD_MAP("SubMeshRes",f_field_function_tuple_m_transform);
		
		
		ClassFunctionTuple * f_class_function_tuple_SubMeshRes = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeSubMeshResOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeSubMeshResOperator::getSubMeshResBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeSubMeshResOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("SubMeshRes",f_class_function_tuple_SubMeshRes);
		}
		void TypeWrapperRegister_MeshComponentRes()
		{
			FieldFunctionTuple * f_field_function_tuple_m_sub_meshes = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMeshComponentResOperator::set_m_sub_meshes,
			&TypeFieldReflectionOperator::TypeMeshComponentResOperator::get_m_sub_meshes,
			&TypeFieldReflectionOperator::TypeMeshComponentResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMeshComponentResOperator::getFieldName_m_sub_meshes,
			&TypeFieldReflectionOperator::TypeMeshComponentResOperator::getFieldTypeName_m_sub_meshes,
			&TypeFieldReflectionOperator::TypeMeshComponentResOperator::isArray_m_sub_meshes
			);
			REGISTER_TO_FIELD_MAP("MeshComponentRes",f_field_function_tuple_m_sub_meshes);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLSubMeshResR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLSubMeshResROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLSubMeshResROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLSubMeshResROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLSubMeshResROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLSubMeshResROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLSubMeshResR",array_function_tuple_stdSSvectorLSubMeshResR);
		ClassFunctionTuple * f_class_function_tuple_MeshComponentRes = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeMeshComponentResOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeMeshComponentResOperator::getMeshComponentResBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMeshComponentResOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MeshComponentRes",f_class_function_tuple_MeshComponentRes);
		}
		namespace TypeWrappersRegister
		{
			void SubMeshRes(){TypeWrapperRegister_SubMeshRes();}
			void MeshComponentRes(){TypeWrapperRegister_MeshComponentRes();}
		}
		
	}
}

