#pragma once
#include "runtime\resource\common\object.h"

namespace QYHS
{
	class ObjectDefinitionResource;
	class ObjectInstanceResource;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeObjectDefinitionResourceOperator
			{
			public:
				//class
				//static void getObjectDefinitionResourceBaseClassReflectionInstanceList()
				static const char* getClassName(){return "ObjectDefinitionResource";}
				static void* constructorWithJson(const Json & json_context)
				{
					ObjectDefinitionResource * ret_instance = new ObjectDefinitionResource;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_components(void * instance,void * field_value)
				{
					static_cast<ObjectDefinitionResource*>(instance)->m_components = *static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(field_value);
				}
				static void* get_m_components(void * instance)
				{
					return &(static_cast<ObjectDefinitionResource*>(instance)->m_components);
				}
				static const char* getFieldName_m_components()
				{
					return "m_components";
				}
				static const char* getFieldTypeName_m_components()
				{
					return "std::vector<Reflection::ReflectionPtr<Component>>";
				}
				static bool isArray_m_components()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperatorMacro
		#define ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperatorMacro
		class ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<Reflection::ReflectionPtr<Component>>";}
			static const char* getElementTypeName(){return "Reflection::ReflectionPtr<Component>";}
			static int getSize(void * instance){return (static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance))[index] = *(static_cast<Reflection::ReflectionPtr<Component>*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator
		namespace TypeFieldReflectionOperator
		{
			class TypeObjectInstanceResourceOperator
			{
			public:
				//class
				//static void getObjectInstanceResourceBaseClassReflectionInstanceList()
				static const char* getClassName(){return "ObjectInstanceResource";}
				static void* constructorWithJson(const Json & json_context)
				{
					ObjectInstanceResource * ret_instance = new ObjectInstanceResource;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_name(void * instance,void * field_value)
				{
					static_cast<ObjectInstanceResource*>(instance)->m_name = *static_cast<std::string*>(field_value);
				}
				static void* get_m_name(void * instance)
				{
					return &(static_cast<ObjectInstanceResource*>(instance)->m_name);
				}
				static const char* getFieldName_m_name()
				{
					return "m_name";
				}
				static const char* getFieldTypeName_m_name()
				{
					return "std::string";
				}
				static bool isArray_m_name()
				{
					return false;
				}
				static void set_m_definition(void * instance,void * field_value)
				{
					static_cast<ObjectInstanceResource*>(instance)->m_definition = *static_cast<std::string*>(field_value);
				}
				static void* get_m_definition(void * instance)
				{
					return &(static_cast<ObjectInstanceResource*>(instance)->m_definition);
				}
				static const char* getFieldName_m_definition()
				{
					return "m_definition";
				}
				static const char* getFieldTypeName_m_definition()
				{
					return "std::string";
				}
				static bool isArray_m_definition()
				{
					return false;
				}
				static void set_m_instanced_components(void * instance,void * field_value)
				{
					static_cast<ObjectInstanceResource*>(instance)->m_instanced_components = *static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(field_value);
				}
				static void* get_m_instanced_components(void * instance)
				{
					return &(static_cast<ObjectInstanceResource*>(instance)->m_instanced_components);
				}
				static const char* getFieldName_m_instanced_components()
				{
					return "m_instanced_components";
				}
				static const char* getFieldTypeName_m_instanced_components()
				{
					return "std::vector<Reflection::ReflectionPtr<Component>>";
				}
				static bool isArray_m_instanced_components()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperatorMacro
		#define ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperatorMacro
		class ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<Reflection::ReflectionPtr<Component>>";}
			static const char* getElementTypeName(){return "Reflection::ReflectionPtr<Component>";}
			static int getSize(void * instance){return (static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<Reflection::ReflectionPtr<Component>>*>(instance))[index] = *(static_cast<Reflection::ReflectionPtr<Component>*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_ObjectDefinitionResource()
		{
			FieldFunctionTuple * f_field_function_tuple_m_components = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::set_m_components,
			&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::get_m_components,
			&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::getFieldName_m_components,
			&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::getFieldTypeName_m_components,
			&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::isArray_m_components
			);
			REGISTER_TO_FIELD_MAP("m_components",f_field_function_tuple_m_components);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLReflectionSSReflectionPtrLComponentRR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLReflectionSSReflectionPtrLComponentRR",array_function_tuple_stdSSvectorLReflectionSSReflectionPtrLComponentRR);
		ClassFunctionTuple * f_class_function_tuple_ObjectDefinitionResource = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::getObjectDefinitionResourceBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeObjectDefinitionResourceOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("ObjectDefinitionResource",f_class_function_tuple_ObjectDefinitionResource);
		}
		void TypeWrapperRegister_ObjectInstanceResource()
		{
			FieldFunctionTuple * f_field_function_tuple_m_name = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::set_m_name,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::get_m_name,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getFieldName_m_name,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getFieldTypeName_m_name,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::isArray_m_name
			);
			REGISTER_TO_FIELD_MAP("m_name",f_field_function_tuple_m_name);
			FieldFunctionTuple * f_field_function_tuple_m_definition = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::set_m_definition,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::get_m_definition,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getFieldName_m_definition,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getFieldTypeName_m_definition,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::isArray_m_definition
			);
			REGISTER_TO_FIELD_MAP("m_definition",f_field_function_tuple_m_definition);
			FieldFunctionTuple * f_field_function_tuple_m_instanced_components = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::set_m_instanced_components,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::get_m_instanced_components,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getFieldName_m_instanced_components,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getFieldTypeName_m_instanced_components,
			&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::isArray_m_instanced_components
			);
			REGISTER_TO_FIELD_MAP("m_instanced_components",f_field_function_tuple_m_instanced_components);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLReflectionSSReflectionPtrLComponentRR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLReflectionSSReflectionPtrLComponentRROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLReflectionSSReflectionPtrLComponentRR",array_function_tuple_stdSSvectorLReflectionSSReflectionPtrLComponentRR);
		ClassFunctionTuple * f_class_function_tuple_ObjectInstanceResource = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::getObjectInstanceResourceBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeObjectInstanceResourceOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("ObjectInstanceResource",f_class_function_tuple_ObjectInstanceResource);
		}
		namespace TypeWrappersRegister
		{
			void ObjectDefinitionResource(){TypeWrapperRegister_ObjectDefinitionResource();}
			void ObjectInstanceResource(){TypeWrapperRegister_ObjectInstanceResource();}
		}
		
	}
}

