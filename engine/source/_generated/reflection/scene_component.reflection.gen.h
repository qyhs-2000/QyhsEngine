#pragma once
#include "runtime\function\render\scene_component.h"

namespace qyhs
{
	class AnimComponentTest;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimComponentTestOperator
			{
			public:
				//class
				//static void getAnimComponentTestBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimComponentTest";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimComponentTest * ret_instance = new AnimComponentTest;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimComponentTestBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(qyhs::Component,static_cast<AnimComponentTest*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimComponentTest*)instance_ptr);
				}
				//field
				static void set_test_int(void * instance,void * field_value)
				{
					static_cast<AnimComponentTest*>(instance)->test_int = *static_cast<int*>(field_value);
				}
				static void* get_test_int(void * instance)
				{
					return &(static_cast<AnimComponentTest*>(instance)->test_int);
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

		

		void TypeWrapperRegister_AnimComponentTest()
		{
			FieldFunctionTuple * f_field_function_tuple_test_int = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimComponentTestOperator::set_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTestOperator::get_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTestOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimComponentTestOperator::getFieldName_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTestOperator::getFieldTypeName_test_int,
			&TypeFieldReflectionOperator::TypeAnimComponentTestOperator::isArray_test_int
			);
			REGISTER_TO_FIELD_MAP("AnimComponentTest",f_field_function_tuple_test_int);
		
		
		ClassFunctionTuple * f_class_function_tuple_AnimComponentTest = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimComponentTestOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimComponentTestOperator::getAnimComponentTestBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimComponentTestOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimComponentTest",f_class_function_tuple_AnimComponentTest);
		}
		namespace TypeWrappersRegister
		{
			void AnimComponentTest(){TypeWrapperRegister_AnimComponentTest();}
		}
		
	}
}

