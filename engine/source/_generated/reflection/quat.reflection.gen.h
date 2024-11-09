#pragma once
#include "runtime\core\math\quat.h"

namespace QYHS
{
	class Quaternion;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeQuaternionOperator
			{
			public:
				//class
				//static void getQuaternionBaseClassReflectionInstanceList()
				static const char* getClassName(){return "Quaternion";}
				static void* constructorWithJson(const Json & json_context)
				{
					Quaternion * ret_instance = new Quaternion;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getQuaternionBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				//field
				static void set_w(void * instance,void * field_value)
				{
					static_cast<Quaternion*>(instance)->w = *static_cast<float*>(field_value);
				}
				static void* get_w(void * instance)
				{
					return &(static_cast<Quaternion*>(instance)->w);
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
				static void set_x(void * instance,void * field_value)
				{
					static_cast<Quaternion*>(instance)->x = *static_cast<float*>(field_value);
				}
				static void* get_x(void * instance)
				{
					return &(static_cast<Quaternion*>(instance)->x);
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
					static_cast<Quaternion*>(instance)->y = *static_cast<float*>(field_value);
				}
				static void* get_y(void * instance)
				{
					return &(static_cast<Quaternion*>(instance)->y);
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
					static_cast<Quaternion*>(instance)->z = *static_cast<float*>(field_value);
				}
				static void* get_z(void * instance)
				{
					return &(static_cast<Quaternion*>(instance)->z);
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

		

		void TypeWrapperRegister_Quaternion()
		{
			FieldFunctionTuple * f_field_function_tuple_w = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeQuaternionOperator::set_w,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::get_w,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getClassName,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldName_w,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldTypeName_w,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::isArray_w
			);
			REGISTER_TO_FIELD_MAP("Quaternion",f_field_function_tuple_w);
			FieldFunctionTuple * f_field_function_tuple_x = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeQuaternionOperator::set_x,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::get_x,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getClassName,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldName_x,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldTypeName_x,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::isArray_x
			);
			REGISTER_TO_FIELD_MAP("Quaternion",f_field_function_tuple_x);
			FieldFunctionTuple * f_field_function_tuple_y = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeQuaternionOperator::set_y,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::get_y,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getClassName,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldName_y,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldTypeName_y,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::isArray_y
			);
			REGISTER_TO_FIELD_MAP("Quaternion",f_field_function_tuple_y);
			FieldFunctionTuple * f_field_function_tuple_z = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeQuaternionOperator::set_z,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::get_z,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getClassName,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldName_z,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::getFieldTypeName_z,
			&TypeFieldReflectionOperator::TypeQuaternionOperator::isArray_z
			);
			REGISTER_TO_FIELD_MAP("Quaternion",f_field_function_tuple_z);
		
		
		ClassFunctionTuple * f_class_function_tuple_Quaternion = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeQuaternionOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeQuaternionOperator::getQuaternionBaseClassReflectionInstanceList
            //&TypeFieldReflectionOperator::TypeQuaternionOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Quaternion",f_class_function_tuple_Quaternion);
		}
		namespace TypeWrappersRegister
		{
			void Quaternion(){TypeWrapperRegister_Quaternion();}
		}
		
	}
}

