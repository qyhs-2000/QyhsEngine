#pragma once
#include "runtime\function\framework\component\component.h"

namespace QYHS
{
	class Component;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeComponentOperator
			{
			public:
				//class
				//static void getComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "Component";}
				static void* constructorWithJson(const Json & json_context)
				{
					Component * ret_instance = new Component;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_Component()
		{
		
		
		
		ClassFunctionTuple * f_class_function_tuple_Component = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeComponentOperator::getComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeComponentOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Component",f_class_function_tuple_Component);
		}
		namespace TypeWrappersRegister
		{
			void Component(){TypeWrapperRegister_Component();}
		}
		
	}
}
