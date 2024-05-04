#pragma once
#include "runtime\core\math\vector3.h"

namespace QYHS
{
	class Vector3;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeVector3Operator
			{
			public:
				//class
				//static void getVector3BaseClassReflectionInstanceList()
				static const char* getClassName(){return "Vector3";}
				static void* constructorWithJson(const Json & json_context)
				{
					Vector3 * ret_instance = new Vector3;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_x(void * instance,void * field_value)
				{
					static_cast<Vector3*>(instance)->x = *static_cast<float*>(field_value);
				}
				static void* get_x(void * instance)
				{
					return &(static_cast<Vector3*>(instance)->x);
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
					static_cast<Vector3*>(instance)->y = *static_cast<float*>(field_value);
				}
				static void* get_y(void * instance)
				{
					return &(static_cast<Vector3*>(instance)->y);
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
				static void set_z(void * instance,void * field_value)
				{
					static_cast<Vector3*>(instance)->z = *static_cast<float*>(field_value);
				}
				static void* get_z(void * instance)
				{
					return &(static_cast<Vector3*>(instance)->z);
				}
				static const char* getFieldName_z()
				{
					return "z";
				}
				static const char* getFieldTypeName_z()
				{
					return "float";
				}
				static bool isArray_z()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_Vector3()
		{
			FieldFunctionTuple * f_field_function_tuple_x = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector3Operator::set_x,
			&TypeFieldReflectionOperator::TypeVector3Operator::get_x,
			&TypeFieldReflectionOperator::TypeVector3Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector3Operator::getFieldName_x,
			&TypeFieldReflectionOperator::TypeVector3Operator::getFieldTypeName_x,
			&TypeFieldReflectionOperator::TypeVector3Operator::isArray_x
			);
			REGISTER_TO_FIELD_MAP("x",f_field_function_tuple_x);
			FieldFunctionTuple * f_field_function_tuple_y = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector3Operator::set_y,
			&TypeFieldReflectionOperator::TypeVector3Operator::get_y,
			&TypeFieldReflectionOperator::TypeVector3Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector3Operator::getFieldName_y,
			&TypeFieldReflectionOperator::TypeVector3Operator::getFieldTypeName_y,
			&TypeFieldReflectionOperator::TypeVector3Operator::isArray_y
			);
			REGISTER_TO_FIELD_MAP("y",f_field_function_tuple_y);
			FieldFunctionTuple * f_field_function_tuple_z = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector3Operator::set_z,
			&TypeFieldReflectionOperator::TypeVector3Operator::get_z,
			&TypeFieldReflectionOperator::TypeVector3Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector3Operator::getFieldName_z,
			&TypeFieldReflectionOperator::TypeVector3Operator::getFieldTypeName_z,
			&TypeFieldReflectionOperator::TypeVector3Operator::isArray_z
			);
			REGISTER_TO_FIELD_MAP("z",f_field_function_tuple_z);
		
		
		ClassFunctionTuple * f_class_function_tuple_Vector3 = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeVector3Operator::getVector3BaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeVector3Operator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeVector3Operator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Vector3",f_class_function_tuple_Vector3);
		}
		namespace TypeWrappersRegister
		{
			void Vector3(){TypeWrapperRegister_Vector3();}
		}
		
	}
}

