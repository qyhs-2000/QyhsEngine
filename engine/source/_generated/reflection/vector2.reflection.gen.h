#pragma once
#include "runtime\core\math\vector2.h"

namespace QYHS
{
	class Vector2;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeVector2Operator
			{
			public:
				//class
				//static void getVector2BaseClassReflectionInstanceList()
				static const char* getClassName(){return "Vector2";}
				static void* constructorWithJson(const Json & json_context)
				{
					Vector2 * ret_instance = new Vector2;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getVector2BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(Vector2*)instance_ptr);
				}
				//field
				static void set_x(void * instance,void * field_value)
				{
					static_cast<Vector2*>(instance)->x = *static_cast<float*>(field_value);
				}
				static void* get_x(void * instance)
				{
					return &(static_cast<Vector2*>(instance)->x);
				}
				static const char* getFieldName_x()
				{
					return "x";
				}
				static const char* getFieldTypeName_x()
				{
					return "float";
				}
				static bool isArray_x()
				{
					return false;
				}
				static void set_y(void * instance,void * field_value)
				{
					static_cast<Vector2*>(instance)->y = *static_cast<float*>(field_value);
				}
				static void* get_y(void * instance)
				{
					return &(static_cast<Vector2*>(instance)->y);
				}
				static const char* getFieldName_y()
				{
					return "y";
				}
				static const char* getFieldTypeName_y()
				{
					return "float";
				}
				static bool isArray_y()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_Vector2()
		{
			FieldFunctionTuple * f_field_function_tuple_x = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector2Operator::set_x,
			&TypeFieldReflectionOperator::TypeVector2Operator::get_x,
			&TypeFieldReflectionOperator::TypeVector2Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector2Operator::getFieldName_x,
			&TypeFieldReflectionOperator::TypeVector2Operator::getFieldTypeName_x,
			&TypeFieldReflectionOperator::TypeVector2Operator::isArray_x
			);
			REGISTER_TO_FIELD_MAP("Vector2",f_field_function_tuple_x);
			FieldFunctionTuple * f_field_function_tuple_y = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector2Operator::set_y,
			&TypeFieldReflectionOperator::TypeVector2Operator::get_y,
			&TypeFieldReflectionOperator::TypeVector2Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector2Operator::getFieldName_y,
			&TypeFieldReflectionOperator::TypeVector2Operator::getFieldTypeName_y,
			&TypeFieldReflectionOperator::TypeVector2Operator::isArray_y
			);
			REGISTER_TO_FIELD_MAP("Vector2",f_field_function_tuple_y);
		
		
		ClassFunctionTuple * f_class_function_tuple_Vector2 = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeVector2Operator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeVector2Operator::getVector2BaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeVector2Operator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Vector2",f_class_function_tuple_Vector2);
		}
		namespace TypeWrappersRegister
		{
			void Vector2(){TypeWrapperRegister_Vector2();}
		}
		
	}
}

