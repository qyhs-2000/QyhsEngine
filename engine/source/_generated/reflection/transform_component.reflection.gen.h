#pragma once
#include "runtime\function\framework\component\transform\transform_component.h"

namespace QYHS
{
	class TransformComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeTransformComponentOperator
			{
			public:
				//class
				//static void getTransformComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "TransformComponent";}
				static void* constructorWithJson(const Json & json_context)
				{
					TransformComponent * ret_instance = new TransformComponent;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getTransformComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(QYHS::Component,static_cast<TransformComponent*>(instance));
					}
					return count;	
				}
				//field
				static void set_m_transform(void * instance,void * field_value)
				{
					static_cast<TransformComponent*>(instance)->m_transform = *static_cast<Transform*>(field_value);
				}
				static void* get_m_transform(void * instance)
				{
					return &(static_cast<TransformComponent*>(instance)->m_transform);
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

		

		void TypeWrapperRegister_TransformComponent()
		{
			FieldFunctionTuple * f_field_function_tuple_m_transform = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeTransformComponentOperator::set_m_transform,
			&TypeFieldReflectionOperator::TypeTransformComponentOperator::get_m_transform,
			&TypeFieldReflectionOperator::TypeTransformComponentOperator::getClassName,
			&TypeFieldReflectionOperator::TypeTransformComponentOperator::getFieldName_m_transform,
			&TypeFieldReflectionOperator::TypeTransformComponentOperator::getFieldTypeName_m_transform,
			&TypeFieldReflectionOperator::TypeTransformComponentOperator::isArray_m_transform
			);
			REGISTER_TO_FIELD_MAP("TransformComponent",f_field_function_tuple_m_transform);
		
		
		ClassFunctionTuple * f_class_function_tuple_TransformComponent = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeTransformComponentOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeTransformComponentOperator::getTransformComponentBaseClassReflectionInstanceList
            //&TypeFieldReflectionOperator::TypeTransformComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("TransformComponent",f_class_function_tuple_TransformComponent);
		}
		namespace TypeWrappersRegister
		{
			void TransformComponent(){TypeWrapperRegister_TransformComponent();}
		}
		
	}
}

