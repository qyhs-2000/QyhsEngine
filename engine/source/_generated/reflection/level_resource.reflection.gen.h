#pragma once
#include "runtime\resource\type\level_resource.h"

namespace QYHS
{
	class LevelResource;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeLevelResourceOperator
			{
			public:
				//class
				//static void getLevelResourceBaseClassReflectionInstanceList()
				static const char* getClassName(){return "LevelResource";}
				static void* constructorWithJson(const Json & json_context)
				{
					LevelResource * ret_instance = new LevelResource;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_gravity(void * instance,void * field_value)
				{
					static_cast<LevelResource*>(instance)->m_gravity = *static_cast<Vector3*>(field_value);
				}
				static void* get_m_gravity(void * instance)
				{
					return &(static_cast<LevelResource*>(instance)->m_gravity);
				}
				static const char* getFieldName_m_gravity()
				{
					return "m_gravity";
				}
				static const char* getFieldTypeName_m_gravity()
				{
					return "Vector3";
				}
				static bool isArray_m_gravity()
				{
					return false;
				}
				static void set_m_character_name(void * instance,void * field_value)
				{
					static_cast<LevelResource*>(instance)->m_character_name = *static_cast<std::string*>(field_value);
				}
				static void* get_m_character_name(void * instance)
				{
					return &(static_cast<LevelResource*>(instance)->m_character_name);
				}
				static const char* getFieldName_m_character_name()
				{
					return "m_character_name";
				}
				static const char* getFieldTypeName_m_character_name()
				{
					return "std::string";
				}
				static bool isArray_m_character_name()
				{
					return false;
				}
				static void set_m_test_name(void * instance,void * field_value)
				{
					static_cast<LevelResource*>(instance)->m_test_name = *static_cast<std::string*>(field_value);
				}
				static void* get_m_test_name(void * instance)
				{
					return &(static_cast<LevelResource*>(instance)->m_test_name);
				}
				static const char* getFieldName_m_test_name()
				{
					return "m_test_name";
				}
				static const char* getFieldTypeName_m_test_name()
				{
					return "std::string";
				}
				static bool isArray_m_test_name()
				{
					return false;
				}
				static void set_m_test_name2(void * instance,void * field_value)
				{
					static_cast<LevelResource*>(instance)->m_test_name2 = *static_cast<std::string*>(field_value);
				}
				static void* get_m_test_name2(void * instance)
				{
					return &(static_cast<LevelResource*>(instance)->m_test_name2);
				}
				static const char* getFieldName_m_test_name2()
				{
					return "m_test_name2";
				}
				static const char* getFieldTypeName_m_test_name2()
				{
					return "std::string";
				}
				static bool isArray_m_test_name2()
				{
					return false;
				}
				static void set_m_objects(void * instance,void * field_value)
				{
					static_cast<LevelResource*>(instance)->m_objects = *static_cast<std::vector<ObjectInstanceResource>*>(field_value);
				}
				static void* get_m_objects(void * instance)
				{
					return &(static_cast<LevelResource*>(instance)->m_objects);
				}
				static const char* getFieldName_m_objects()
				{
					return "m_objects";
				}
				static const char* getFieldTypeName_m_objects()
				{
					return "std::vector<ObjectInstanceResource>";
				}
				static bool isArray_m_objects()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLObjectInstanceResourceROperatorMacro
		#define ArraystdSSvectorLObjectInstanceResourceROperatorMacro
		class ArraystdSSvectorLObjectInstanceResourceROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<ObjectInstanceResource>";}
			static const char* getElementTypeName(){return "ObjectInstanceResource";}
			static int getSize(void * instance){return (static_cast<std::vector<ObjectInstanceResource>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<ObjectInstanceResource>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<ObjectInstanceResource>*>(instance))[index] = *(static_cast<ObjectInstanceResource*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_LevelResource()
		{
			FieldFunctionTuple * f_field_function_tuple_m_gravity = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::set_m_gravity,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::get_m_gravity,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldName_m_gravity,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldTypeName_m_gravity,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::isArray_m_gravity
			);
			REGISTER_TO_FIELD_MAP("m_gravity",f_field_function_tuple_m_gravity);
			FieldFunctionTuple * f_field_function_tuple_m_character_name = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::set_m_character_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::get_m_character_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldName_m_character_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldTypeName_m_character_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::isArray_m_character_name
			);
			REGISTER_TO_FIELD_MAP("m_character_name",f_field_function_tuple_m_character_name);
			FieldFunctionTuple * f_field_function_tuple_m_test_name = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::set_m_test_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::get_m_test_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldName_m_test_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldTypeName_m_test_name,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::isArray_m_test_name
			);
			REGISTER_TO_FIELD_MAP("m_test_name",f_field_function_tuple_m_test_name);
			FieldFunctionTuple * f_field_function_tuple_m_test_name2 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::set_m_test_name2,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::get_m_test_name2,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldName_m_test_name2,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldTypeName_m_test_name2,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::isArray_m_test_name2
			);
			REGISTER_TO_FIELD_MAP("m_test_name2",f_field_function_tuple_m_test_name2);
			FieldFunctionTuple * f_field_function_tuple_m_objects = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::set_m_objects,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::get_m_objects,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldName_m_objects,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::getFieldTypeName_m_objects,
			&TypeFieldReflectionOperator::TypeLevelResourceOperator::isArray_m_objects
			);
			REGISTER_TO_FIELD_MAP("m_objects",f_field_function_tuple_m_objects);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLObjectInstanceResourceR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLObjectInstanceResourceROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLObjectInstanceResourceROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLObjectInstanceResourceROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLObjectInstanceResourceROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLObjectInstanceResourceROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLObjectInstanceResourceR",array_function_tuple_stdSSvectorLObjectInstanceResourceR);
		ClassFunctionTuple * f_class_function_tuple_LevelResource = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeLevelResourceOperator::getLevelResourceBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeLevelResourceOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeLevelResourceOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("LevelResource",f_class_function_tuple_LevelResource);
		}
		namespace TypeWrappersRegister
		{
			void LevelResource(){TypeWrapperRegister_LevelResource();}
		}
		
	}
}
