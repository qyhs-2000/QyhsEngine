#pragma once
#include "runtime\core\math\matrix4x4.h"

namespace QYHS
{
	class Matrix4x4_;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeMatrix4x4_Operator
			{
			public:
				//class
				//static void getMatrix4x4_BaseClassReflectionInstanceList()
				static const char* getClassName(){return "Matrix4x4_";}
				static void* constructorWithJson(const Json & json_context)
				{
					Matrix4x4_ * ret_instance = new Matrix4x4_;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getMatrix4x4_BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(Matrix4x4_*)instance_ptr);
				}
				//field
				static void set_v0(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v0 = *static_cast<float*>(field_value);
				}
				static void* get_v0(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v0);
				}
				static const char* getFieldName_v0()
				{
					return "v0";
				}
				static const char* getFieldTypeName_v0()
				{
					return "float";
				}
				static bool isArray_v0()
				{
					return false;
				}
				static void set_v1(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v1 = *static_cast<float*>(field_value);
				}
				static void* get_v1(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v1);
				}
				static const char* getFieldName_v1()
				{
					return "v1";
				}
				static const char* getFieldTypeName_v1()
				{
					return "float";
				}
				static bool isArray_v1()
				{
					return false;
				}
				static void set_v2(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v2 = *static_cast<float*>(field_value);
				}
				static void* get_v2(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v2);
				}
				static const char* getFieldName_v2()
				{
					return "v2";
				}
				static const char* getFieldTypeName_v2()
				{
					return "float";
				}
				static bool isArray_v2()
				{
					return false;
				}
				static void set_v3(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v3 = *static_cast<float*>(field_value);
				}
				static void* get_v3(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v3);
				}
				static const char* getFieldName_v3()
				{
					return "v3";
				}
				static const char* getFieldTypeName_v3()
				{
					return "float";
				}
				static bool isArray_v3()
				{
					return false;
				}
				static void set_v4(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v4 = *static_cast<float*>(field_value);
				}
				static void* get_v4(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v4);
				}
				static const char* getFieldName_v4()
				{
					return "v4";
				}
				static const char* getFieldTypeName_v4()
				{
					return "float";
				}
				static bool isArray_v4()
				{
					return false;
				}
				static void set_v5(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v5 = *static_cast<float*>(field_value);
				}
				static void* get_v5(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v5);
				}
				static const char* getFieldName_v5()
				{
					return "v5";
				}
				static const char* getFieldTypeName_v5()
				{
					return "float";
				}
				static bool isArray_v5()
				{
					return false;
				}
				static void set_v6(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v6 = *static_cast<float*>(field_value);
				}
				static void* get_v6(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v6);
				}
				static const char* getFieldName_v6()
				{
					return "v6";
				}
				static const char* getFieldTypeName_v6()
				{
					return "float";
				}
				static bool isArray_v6()
				{
					return false;
				}
				static void set_v7(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v7 = *static_cast<float*>(field_value);
				}
				static void* get_v7(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v7);
				}
				static const char* getFieldName_v7()
				{
					return "v7";
				}
				static const char* getFieldTypeName_v7()
				{
					return "float";
				}
				static bool isArray_v7()
				{
					return false;
				}
				static void set_v8(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v8 = *static_cast<float*>(field_value);
				}
				static void* get_v8(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v8);
				}
				static const char* getFieldName_v8()
				{
					return "v8";
				}
				static const char* getFieldTypeName_v8()
				{
					return "float";
				}
				static bool isArray_v8()
				{
					return false;
				}
				static void set_v9(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v9 = *static_cast<float*>(field_value);
				}
				static void* get_v9(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v9);
				}
				static const char* getFieldName_v9()
				{
					return "v9";
				}
				static const char* getFieldTypeName_v9()
				{
					return "float";
				}
				static bool isArray_v9()
				{
					return false;
				}
				static void set_v10(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v10 = *static_cast<float*>(field_value);
				}
				static void* get_v10(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v10);
				}
				static const char* getFieldName_v10()
				{
					return "v10";
				}
				static const char* getFieldTypeName_v10()
				{
					return "float";
				}
				static bool isArray_v10()
				{
					return false;
				}
				static void set_v11(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v11 = *static_cast<float*>(field_value);
				}
				static void* get_v11(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v11);
				}
				static const char* getFieldName_v11()
				{
					return "v11";
				}
				static const char* getFieldTypeName_v11()
				{
					return "float";
				}
				static bool isArray_v11()
				{
					return false;
				}
				static void set_v12(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v12 = *static_cast<float*>(field_value);
				}
				static void* get_v12(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v12);
				}
				static const char* getFieldName_v12()
				{
					return "v12";
				}
				static const char* getFieldTypeName_v12()
				{
					return "float";
				}
				static bool isArray_v12()
				{
					return false;
				}
				static void set_v13(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v13 = *static_cast<float*>(field_value);
				}
				static void* get_v13(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v13);
				}
				static const char* getFieldName_v13()
				{
					return "v13";
				}
				static const char* getFieldTypeName_v13()
				{
					return "float";
				}
				static bool isArray_v13()
				{
					return false;
				}
				static void set_v14(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v14 = *static_cast<float*>(field_value);
				}
				static void* get_v14(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v14);
				}
				static const char* getFieldName_v14()
				{
					return "v14";
				}
				static const char* getFieldTypeName_v14()
				{
					return "float";
				}
				static bool isArray_v14()
				{
					return false;
				}
				static void set_v15(void * instance,void * field_value)
				{
					static_cast<Matrix4x4_*>(instance)->v15 = *static_cast<float*>(field_value);
				}
				static void* get_v15(void * instance)
				{
					return &(static_cast<Matrix4x4_*>(instance)->v15);
				}
				static const char* getFieldName_v15()
				{
					return "v15";
				}
				static const char* getFieldTypeName_v15()
				{
					return "float";
				}
				static bool isArray_v15()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_Matrix4x4_()
		{
			FieldFunctionTuple * f_field_function_tuple_v0 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v0,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v0,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v0,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v0,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v0
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v0);
			FieldFunctionTuple * f_field_function_tuple_v1 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v1,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v1,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v1,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v1,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v1
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v1);
			FieldFunctionTuple * f_field_function_tuple_v2 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v2,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v2,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v2,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v2,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v2
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v2);
			FieldFunctionTuple * f_field_function_tuple_v3 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v3,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v3,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v3,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v3,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v3
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v3);
			FieldFunctionTuple * f_field_function_tuple_v4 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v4,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v4,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v4,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v4,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v4
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v4);
			FieldFunctionTuple * f_field_function_tuple_v5 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v5,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v5,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v5,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v5,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v5
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v5);
			FieldFunctionTuple * f_field_function_tuple_v6 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v6,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v6,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v6,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v6,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v6
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v6);
			FieldFunctionTuple * f_field_function_tuple_v7 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v7,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v7,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v7,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v7,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v7
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v7);
			FieldFunctionTuple * f_field_function_tuple_v8 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v8,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v8,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v8,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v8,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v8
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v8);
			FieldFunctionTuple * f_field_function_tuple_v9 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v9,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v9,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v9,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v9,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v9
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v9);
			FieldFunctionTuple * f_field_function_tuple_v10 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v10,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v10,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v10,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v10,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v10
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v10);
			FieldFunctionTuple * f_field_function_tuple_v11 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v11,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v11,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v11,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v11,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v11
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v11);
			FieldFunctionTuple * f_field_function_tuple_v12 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v12,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v12,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v12,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v12,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v12
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v12);
			FieldFunctionTuple * f_field_function_tuple_v13 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v13,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v13,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v13,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v13,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v13
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v13);
			FieldFunctionTuple * f_field_function_tuple_v14 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v14,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v14,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v14,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v14,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v14
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v14);
			FieldFunctionTuple * f_field_function_tuple_v15 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::set_v15,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::get_v15,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getClassName,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldName_v15,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getFieldTypeName_v15,
			&TypeFieldReflectionOperator::TypeMatrix4x4_Operator::isArray_v15
			);
			REGISTER_TO_FIELD_MAP("Matrix4x4_",f_field_function_tuple_v15);
		
		
		ClassFunctionTuple * f_class_function_tuple_Matrix4x4_ = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeMatrix4x4_Operator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeMatrix4x4_Operator::getMatrix4x4_BaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMatrix4x4_Operator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Matrix4x4_",f_class_function_tuple_Matrix4x4_);
		}
		namespace TypeWrappersRegister
		{
			void Matrix4x4_(){TypeWrapperRegister_Matrix4x4_();}
		}
		
	}
}

