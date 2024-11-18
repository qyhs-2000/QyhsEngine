#pragma once
#include "runtime\function\framework\component\motor\motor_component.h"

namespace QYHS
{
	class MotorComponentRes;
	class MotorComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeMotorComponentResOperator
			{
			public:
				//class
				//static void getMotorComponentResBaseClassReflectionInstanceList()
				static const char* getClassName(){return "MotorComponentRes";}
				static void* constructorWithJson(const Json & json_context)
				{
					MotorComponentRes * ret_instance = new MotorComponentRes;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getMotorComponentResBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(MotorComponentRes*)instance_ptr);
				}
				//field
				static void set_move_speed(void * instance,void * field_value)
				{
					static_cast<MotorComponentRes*>(instance)->move_speed = *static_cast<float*>(field_value);
				}
				static void* get_move_speed(void * instance)
				{
					return &(static_cast<MotorComponentRes*>(instance)->move_speed);
				}
				static const char* getFieldName_move_speed()
				{
					return "move_speed";
				}
				static const char* getFieldTypeName_move_speed()
				{
					return "float";
				}
				static bool isArray_move_speed()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
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
				static int getMotorComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(QYHS::Component,static_cast<MotorComponent*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(MotorComponent*)instance_ptr);
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
				static void set_m_motor_res(void * instance,void * field_value)
				{
					static_cast<MotorComponent*>(instance)->m_motor_res = *static_cast<MotorComponentRes*>(field_value);
				}
				static void* get_m_motor_res(void * instance)
				{
					return &(static_cast<MotorComponent*>(instance)->m_motor_res);
				}
				static const char* getFieldName_m_motor_res()
				{
					return "m_motor_res";
				}
				static const char* getFieldTypeName_m_motor_res()
				{
					return "MotorComponentRes";
				}
				static bool isArray_m_motor_res()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_MotorComponentRes()
		{
			FieldFunctionTuple * f_field_function_tuple_move_speed = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMotorComponentResOperator::set_move_speed,
			&TypeFieldReflectionOperator::TypeMotorComponentResOperator::get_move_speed,
			&TypeFieldReflectionOperator::TypeMotorComponentResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMotorComponentResOperator::getFieldName_move_speed,
			&TypeFieldReflectionOperator::TypeMotorComponentResOperator::getFieldTypeName_move_speed,
			&TypeFieldReflectionOperator::TypeMotorComponentResOperator::isArray_move_speed
			);
			REGISTER_TO_FIELD_MAP("MotorComponentRes",f_field_function_tuple_move_speed);
		
		
		ClassFunctionTuple * f_class_function_tuple_MotorComponentRes = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeMotorComponentResOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeMotorComponentResOperator::getMotorComponentResBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMotorComponentResOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MotorComponentRes",f_class_function_tuple_MotorComponentRes);
		}
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
			REGISTER_TO_FIELD_MAP("MotorComponent",f_field_function_tuple_m_transform);
			FieldFunctionTuple * f_field_function_tuple_m_motor_res = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::set_m_motor_res,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::get_m_motor_res,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::getFieldName_m_motor_res,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::getFieldTypeName_m_motor_res,
			&TypeFieldReflectionOperator::TypeMotorComponentOperator::isArray_m_motor_res
			);
			REGISTER_TO_FIELD_MAP("MotorComponent",f_field_function_tuple_m_motor_res);
		
		
		ClassFunctionTuple * f_class_function_tuple_MotorComponent = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeMotorComponentOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeMotorComponentOperator::getMotorComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMotorComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MotorComponent",f_class_function_tuple_MotorComponent);
		}
		namespace TypeWrappersRegister
		{
			void MotorComponentRes(){TypeWrapperRegister_MotorComponentRes();}
			void MotorComponent(){TypeWrapperRegister_MotorComponent();}
		}
		
	}
}

