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
			REGISTER_TO_FIELD_MAP("m_mesh_res",f_field_function_tuple_m_mesh_res);
		
		
		ClassFunctionTuple * f_class_function_tuple_MeshComponent = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeMeshComponentOperator::getMeshComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMeshComponentOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeMeshComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MeshComponent",f_class_function_tuple_MeshComponent);
		}
		namespace TypeWrappersRegister
		{
			void MeshComponent(){TypeWrapperRegister_MeshComponent();}
		}
		
	}
}

