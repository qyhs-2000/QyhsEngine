#pragma once
#include "runtime\function\framework\component\mesh\mesh_component.h"

namespace QYHS
{
	class MeshComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeMeshComponentOperator
			{
			public:
				//class
				//static void getMeshComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "MeshComponent";}
				static void* constructorWithJson(const Json & json_context)
				{
					MeshComponent * ret_instance = new MeshComponent;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getMeshComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(QYHS::Component,static_cast<MeshComponent*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(MeshComponent*)instance_ptr);
				}
				//field
				static void set_m_mesh_res(void * instance,void * field_value)
				{
					static_cast<MeshComponent*>(instance)->m_mesh_res = *static_cast<MeshComponentRes*>(field_value);
				}
				static void* get_m_mesh_res(void * instance)
				{
					return &(static_cast<MeshComponent*>(instance)->m_mesh_res);
				}
				static const char* getFieldName_m_mesh_res()
				{
					return "m_mesh_res";
				}
				static const char* getFieldTypeName_m_mesh_res()
				{
					return "MeshComponentRes";
				}
				static bool isArray_m_mesh_res()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_MeshComponent()
		{
			FieldFunctionTuple * f_field_function_tuple_m_mesh_res = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMeshComponentOperator::set_m_mesh_res,
			&TypeFieldReflectionOperator::TypeMeshComponentOperator::get_m_mesh_res,
			&TypeFieldReflectionOperator::TypeMeshComponentOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMeshComponentOperator::getFieldName_m_mesh_res,
			&TypeFieldReflectionOperator::TypeMeshComponentOperator::getFieldTypeName_m_mesh_res,
			&TypeFieldReflectionOperator::TypeMeshComponentOperator::isArray_m_mesh_res
			);
			REGISTER_TO_FIELD_MAP("MeshComponent",f_field_function_tuple_m_mesh_res);
		
		
		ClassFunctionTuple * f_class_function_tuple_MeshComponent = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeMeshComponentOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeMeshComponentOperator::getMeshComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMeshComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MeshComponent",f_class_function_tuple_MeshComponent);
		}
		namespace TypeWrappersRegister
		{
			void MeshComponent(){TypeWrapperRegister_MeshComponent();}
		}
		
	}
}

