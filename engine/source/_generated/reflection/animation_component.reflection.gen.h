#pragma once
#include "runtime\function\framework\component\animation\animation_component.h"

namespace QYHS
{
	class AnimationComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationComponentOperator
			{
			public:
				//class
				//static void getAnimationComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationComponent";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationComponent * ret_instance = new AnimationComponent;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationComponent*)instance_ptr);
				}
				//field
				static void set_m_animation_res(void * instance,void * field_value)
				{
					static_cast<AnimationComponent*>(instance)->m_animation_res = *static_cast<AnimationComponentRes*>(field_value);
				}
				static void* get_m_animation_res(void * instance)
				{
					return &(static_cast<AnimationComponent*>(instance)->m_animation_res);
				}
				static const char* getFieldName_m_animation_res()
				{
					return "m_animation_res";
				}
				static const char* getFieldTypeName_m_animation_res()
				{
					return "AnimationComponentRes";
				}
				static bool isArray_m_animation_res()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_AnimationComponent()
		{
			FieldFunctionTuple * f_field_function_tuple_m_animation_res = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationComponentOperator::set_m_animation_res,
			&TypeFieldReflectionOperator::TypeAnimationComponentOperator::get_m_animation_res,
			&TypeFieldReflectionOperator::TypeAnimationComponentOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationComponentOperator::getFieldName_m_animation_res,
			&TypeFieldReflectionOperator::TypeAnimationComponentOperator::getFieldTypeName_m_animation_res,
			&TypeFieldReflectionOperator::TypeAnimationComponentOperator::isArray_m_animation_res
			);
			REGISTER_TO_FIELD_MAP("AnimationComponent",f_field_function_tuple_m_animation_res);
		
		
		ClassFunctionTuple * f_class_function_tuple_AnimationComponent = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationComponentOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationComponentOperator::getAnimationComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationComponent",f_class_function_tuple_AnimationComponent);
		}
		namespace TypeWrappersRegister
		{
			void AnimationComponent(){TypeWrapperRegister_AnimationComponent();}
		}
		
	}
}

