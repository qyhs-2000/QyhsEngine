#pragma once
#include "runtime\function\framework\component\animation\animation_skeleton_map.h"

namespace qyhs
{
	class AnimationSkeletonMap;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeAnimationSkeletonMapOperator
			{
			public:
				//class
				//static void getAnimationSkeletonMapBaseClassReflectionInstanceList()
				static const char* getClassName(){return "AnimationSkeletonMap";}
				static void* constructorWithJson(const Json & json_context)
				{
					AnimationSkeletonMap * ret_instance = new AnimationSkeletonMap;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getAnimationSkeletonMapBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(AnimationSkeletonMap*)instance_ptr);
				}
				//field
				static void set_convert(void * instance,void * field_value)
				{
					static_cast<AnimationSkeletonMap*>(instance)->convert = *static_cast<std::vector<int>*>(field_value);
				}
				static void* get_convert(void * instance)
				{
					return &(static_cast<AnimationSkeletonMap*>(instance)->convert);
				}
				static const char* getFieldName_convert()
				{
					return "convert";
				}
				static const char* getFieldTypeName_convert()
				{
					return "std::vector<int>";
				}
				static bool isArray_convert()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLintROperatorMacro
		#define ArraystdSSvectorLintROperatorMacro
		class ArraystdSSvectorLintROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<int>";}
			static const char* getElementTypeName(){return "int";}
			static int getSize(void * instance){return (static_cast<std::vector<int>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<int>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<int>*>(instance))[index] = *(static_cast<int*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_AnimationSkeletonMap()
		{
			FieldFunctionTuple * f_field_function_tuple_convert = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::set_convert,
			&TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::get_convert,
			&TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::getFieldName_convert,
			&TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::getFieldTypeName_convert,
			&TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::isArray_convert
			);
			REGISTER_TO_FIELD_MAP("AnimationSkeletonMap",f_field_function_tuple_convert);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLintR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLintROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLintROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLintROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLintROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLintROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLintR",array_function_tuple_stdSSvectorLintR);
		ClassFunctionTuple * f_class_function_tuple_AnimationSkeletonMap = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::getAnimationSkeletonMapBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeAnimationSkeletonMapOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("AnimationSkeletonMap",f_class_function_tuple_AnimationSkeletonMap);
		}
		namespace TypeWrappersRegister
		{
			void AnimationSkeletonMap(){TypeWrapperRegister_AnimationSkeletonMap();}
		}
		
	}
}

