#pragma once
#include "runtime\function\render\a1.h"

namespace qyhs
{
	class AnimComponentTest111;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimComponentTest111Operator
			{
			public:
				//class
				//static void getAnimComponentTest111BaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimComponentTest111";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimComponentTest111 * ret_instance = new AnimComponentTest111;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimComponentTest111BaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(qyhs::Component,static_cast<AnimComponentTest111*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimComponentTest111*)instance_ptr);
				}
				//field
				static void set_test_int(void * instance,void * field_value)
				{
					static_cast<AnimComponentTest111*>(instance)->test_int = *static_cast<int*>(field_value);
				}
				static void* get_test_int(void * instance)
				{
					return &(static_cast<AnimComponentTest111*>(instance)->test_int);
				}
				static const char* getFieldName_test_int()
				{
					return "test_int";
				}
				static const char* getFieldTypeName_test_int()
				{
					return "int";
				}
				static bool isArray_test_int()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_AnimComponentTest111()
		{
			FieldFunctionTuple * f_field_function_tuple_test_int = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::set_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::get_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::getFieldName_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::getFieldTypeName_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::isArray_test_int
			);
			REGISTER_TO_FIELD_MAP("AnimComponentTest111",f_field_function_tuple_test_int);
		
		
		ClassFunctionTuple * f_class_function_tuple_AnimComponentTest111 = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::getAnimComponentTest111BaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimComponentTest111Operator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimComponentTest111",f_class_function_tuple_AnimComponentTest111);
		}
		namespace TypeWrappersRegister
		{
			void AnimComponentTest111(){TypeWrapperRegister_AnimComponentTest111();}
		}
		
	}
}

