#pragma once
#include "runtime\resource\type\data\material.h"

namespace QYHS
{
	class MaterialRes;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeMaterialResOperator
			{
			public:
				//class
				//static void getMaterialResBaseClassReflectionInstanceList()
				static const char* getClassName(){return "MaterialRes";}
				static void* constructorWithJson(const Json & json_context)
				{
					MaterialRes * ret_instance = new MaterialRes;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_base_colour_texture_file(void * instance,void * field_value)
				{
					static_cast<MaterialRes*>(instance)->m_base_colour_texture_file = *static_cast<std::string*>(field_value);
				}
				static void* get_m_base_colour_texture_file(void * instance)
				{
					return &(static_cast<MaterialRes*>(instance)->m_base_colour_texture_file);
				}
				static const char* getFieldName_m_base_colour_texture_file()
				{
					return "m_base_colour_texture_file";
				}
				static const char* getFieldTypeName_m_base_colour_texture_file()
				{
					return "std::string";
				}
				static bool isArray_m_base_colour_texture_file()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_MaterialRes()
		{
			FieldFunctionTuple * f_field_function_tuple_m_base_colour_texture_file = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMaterialResOperator::set_m_base_colour_texture_file,
			&TypeFieldReflectionOperator::TypeMaterialResOperator::get_m_base_colour_texture_file,
			&TypeFieldReflectionOperator::TypeMaterialResOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMaterialResOperator::getFieldName_m_base_colour_texture_file,
			&TypeFieldReflectionOperator::TypeMaterialResOperator::getFieldTypeName_m_base_colour_texture_file,
			&TypeFieldReflectionOperator::TypeMaterialResOperator::isArray_m_base_colour_texture_file
			);
			REGISTER_TO_FIELD_MAP("m_base_colour_texture_file",f_field_function_tuple_m_base_colour_texture_file);
		
		
		ClassFunctionTuple * f_class_function_tuple_MaterialRes = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeMaterialResOperator::getMaterialResBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMaterialResOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeMaterialResOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MaterialRes",f_class_function_tuple_MaterialRes);
		}
		namespace TypeWrappersRegister
		{
			void MaterialRes(){TypeWrapperRegister_MaterialRes();}
		}
		
	}
}

