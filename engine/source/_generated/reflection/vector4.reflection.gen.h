#pragma once
#include "runtime\core\math\vector4.h"

namespace QYHS
{
	class Vector4;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeVector4Operator
			{
			public:
				//class
				//static void getVector4BaseClassReflectionInstanceList()
				static const char* getClassName(){return "Vector4";}
				static void* constructorWithJson(const Json & json_context)
				{
					Vector4 * ret_instance = new Vector4;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getVector4BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				//field
				static void set_x(void * instance,void * field_value)
				{
					static_cast<Vector4*>(instance)->x = *static_cast<float*>(field_value);
				}
				static void* get_x(void * instance)
				{
					return &(static_cast<Vector4*>(instance)->x);
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
					static_cast<Vector4*>(instance)->y = *static_cast<float*>(field_value);
				}
				static void* get_y(void * instance)
				{
					return &(static_cast<Vector4*>(instance)->y);
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
					static_cast<Vector4*>(instance)->z = *static_cast<float*>(field_value);
				}
				static void* get_z(void * instance)
				{
					return &(static_cast<Vector4*>(instance)->z);
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
				static void set_w(void * instance,void * field_value)
				{
					static_cast<Vector4*>(instance)->w = *static_cast<float*>(field_value);
				}
				static void* get_w(void * instance)
				{
					return &(static_cast<Vector4*>(instance)->w);
				}
				static const char* getFieldName_w()
				{
					return "w";
				}
				static const char* getFieldTypeName_w()
				{
					return "float";
				}
				static bool isArray_w()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_Vector4()
		{
			FieldFunctionTuple * f_field_function_tuple_x = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector4Operator::set_x,
			&TypeFieldReflectionOperator::TypeVector4Operator::get_x,
			&TypeFieldReflectionOperator::TypeVector4Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldName_x,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldTypeName_x,
			&TypeFieldReflectionOperator::TypeVector4Operator::isArray_x
			);
			REGISTER_TO_FIELD_MAP("Vector4",f_field_function_tuple_x);
			FieldFunctionTuple * f_field_function_tuple_y = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector4Operator::set_y,
			&TypeFieldReflectionOperator::TypeVector4Operator::get_y,
			&TypeFieldReflectionOperator::TypeVector4Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldName_y,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldTypeName_y,
			&TypeFieldReflectionOperator::TypeVector4Operator::isArray_y
			);
			REGISTER_TO_FIELD_MAP("Vector4",f_field_function_tuple_y);
			FieldFunctionTuple * f_field_function_tuple_z = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector4Operator::set_z,
			&TypeFieldReflectionOperator::TypeVector4Operator::get_z,
			&TypeFieldReflectionOperator::TypeVector4Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldName_z,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldTypeName_z,
			&TypeFieldReflectionOperator::TypeVector4Operator::isArray_z
			);
			REGISTER_TO_FIELD_MAP("Vector4",f_field_function_tuple_z);
			FieldFunctionTuple * f_field_function_tuple_w = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVector4Operator::set_w,
			&TypeFieldReflectionOperator::TypeVector4Operator::get_w,
			&TypeFieldReflectionOperator::TypeVector4Operator::getClassName,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldName_w,
			&TypeFieldReflectionOperator::TypeVector4Operator::getFieldTypeName_w,
			&TypeFieldReflectionOperator::TypeVector4Operator::isArray_w
			);
			REGISTER_TO_FIELD_MAP("Vector4",f_field_function_tuple_w);
		
		
		ClassFunctionTuple * f_class_function_tuple_Vector4 = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeVector4Operator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeVector4Operator::getVector4BaseClassReflectionInstanceList
            //&TypeFieldReflectionOperator::TypeVector4Operator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Vector4",f_class_function_tuple_Vector4);
		}
		namespace TypeWrappersRegister
		{
			void Vector4(){TypeWrapperRegister_Vector4();}
		}
		
	}
}

