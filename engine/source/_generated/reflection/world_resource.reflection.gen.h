#pragma once
#include "runtime\resource\type\world_resource.h"

namespace QYHS
{
	class WorldResource;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeWorldResourceOperator
			{
			public:
				//class
				//static void getWorldResourceBaseClassReflectionInstanceList()
				static const char* getClassName(){return "WorldResource";}
				static void* constructorWithJson(const Json & json_context)
				{
					WorldResource * ret_instance = new WorldResource;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_default_level_url(void * instance,void * field_value)
				{
					static_cast<WorldResource*>(instance)->m_default_level_url = *static_cast<std::string*>(field_value);
				}
				static void* get_m_default_level_url(void * instance)
				{
					return &(static_cast<WorldResource*>(instance)->m_default_level_url);
				}
				static const char* getFieldName_m_default_level_url()
				{
					return "m_default_level_url";
				}
				static const char* getFieldTypeName_m_default_level_url()
				{
					return "std::string";
				}
				static bool isArray_m_default_level_url()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_WorldResource()
		{
			FieldFunctionTuple * f_field_function_tuple_m_default_level_url = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeWorldResourceOperator::set_m_default_level_url,
			&TypeFieldReflectionOperator::TypeWorldResourceOperator::get_m_default_level_url,
			&TypeFieldReflectionOperator::TypeWorldResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeWorldResourceOperator::getFieldName_m_default_level_url,
			&TypeFieldReflectionOperator::TypeWorldResourceOperator::getFieldTypeName_m_default_level_url,
			&TypeFieldReflectionOperator::TypeWorldResourceOperator::isArray_m_default_level_url
			);
			REGISTER_TO_FIELD_MAP("m_default_level_url",f_field_function_tuple_m_default_level_url);
		
		
		ClassFunctionTuple * f_class_function_tuple_WorldResource = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeWorldResourceOperator::getWorldResourceBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeWorldResourceOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeWorldResourceOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("WorldResource",f_class_function_tuple_WorldResource);
		}
		namespace TypeWrappersRegister
		{
			void WorldResource(){TypeWrapperRegister_WorldResource();}
		}
		
	}
}

