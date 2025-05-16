#pragma once
#include "runtime\function\framework\component\hierarchy\hierarchy_component.h"

namespace qyhs
{
	class HierarchyComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeHierarchyComponentOperator
			{
			public:
				//class
				//static void getHierarchyComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "HierarchyComponent";}
				static void* constructorWithJson(const Json & json_context)
				{
					HierarchyComponent * ret_instance = new HierarchyComponent;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getHierarchyComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(qyhs::Component,static_cast<HierarchyComponent*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(HierarchyComponent*)instance_ptr);
				}
				//field
				
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_HierarchyComponent()
		{
		
		
		
		ClassFunctionTuple * f_class_function_tuple_HierarchyComponent = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeHierarchyComponentOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeHierarchyComponentOperator::getHierarchyComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeHierarchyComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("HierarchyComponent",f_class_function_tuple_HierarchyComponent);
		}
		namespace TypeWrappersRegister
		{
			void HierarchyComponent(){TypeWrapperRegister_HierarchyComponent();}
		}
		
	}
}

