#pragma once
#include "runtime\function\framework\component\animation\animation_component.h"

namespace QYHS
{
	class AnimationComponet;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationComponetOperator
			{
			public:
				//class
				//static void getAnimationComponetBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationComponet";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationComponet * ret_instance = new AnimationComponet;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationComponetBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationComponet*)instance_ptr);
				}
				//field
				
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_AnimationComponet()
		{
		
		
		
		ClassFunctionTuple * f_class_function_tuple_AnimationComponet = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationComponetOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationComponetOperator::getAnimationComponetBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationComponetOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationComponet",f_class_function_tuple_AnimationComponet);
		}
		namespace TypeWrappersRegister
		{
			void AnimationComponet(){TypeWrapperRegister_AnimationComponet();}
		}
		
	}
}

