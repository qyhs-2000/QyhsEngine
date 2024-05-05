#pragma once
#include "runtime\function\framework\component\motor\motor_component.h"

namespace QYHS
{
	class MotorComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeMotorComponentOperator
			{
			public:
				//class
				//static void getMotorComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "MotorComponent";}
				static void* constructorWithJson(const Json & json_context)
				{
					MotorComponent * ret_instance = new MotorComponent;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_transform(void * instance,void * field_value)
				{
					static_cast<MotorComponent*>(instance)->m_transform = *static_cast<Transform*>(field_value);
				}
				static void* get_m_transform(void * instance)
				{
					return &(static_cast<MotorComponent*>(instance)->m_transform);
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

		

		void TypeWrapperRegister_MotorComponent()
		{
			FieldFunctionTuple * f_field_function_tuple_m_transform = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::set_m_transform,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::get_m_transform,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::getFieldName_m_transform,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::getFieldTypeName_m_transform,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::isArray_m_transform
			);
			REGISTER_TO_FIELD_MAP("m_transform",f_field_function_tuple_m_transform);
		
		
		ClassFunctionTuple * f_class_function_tuple_MotorComponent = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeMotorComponentOperator::getMotorComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMotorComponentOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeMotorComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MotorComponent",f_class_function_tuple_MotorComponent);
		}
		namespace TypeWrappersRegister
		{
			void MotorComponent(){TypeWrapperRegister_MotorComponent();}
		}
		
	}
}

