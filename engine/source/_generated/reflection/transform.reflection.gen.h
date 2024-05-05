#pragma once
#include "runtime\core\math\transform.h"

namespace QYHS
{
	class Transform;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeTransformOperator
			{
			public:
				//class
				//static void getTransformBaseClassReflectionInstanceList()
				static const char* getClassName(){return "Transform";}
				static void* constructorWithJson(const Json & json_context)
				{
					Transform * ret_instance = new Transform;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_position(void * instance,void * field_value)
				{
					static_cast<Transform*>(instance)->m_position = *static_cast<Vector3*>(field_value);
				}
				static void* get_m_position(void * instance)
				{
					return &(static_cast<Transform*>(instance)->m_position);
				}
				static const char* getFieldName_m_position()
				{
					return "m_position";
				}
				static const char* getFieldTypeName_m_position()
				{
					return "Vector3";
				}
				static bool isArray_m_position()
				{
					return false;
				}
				static void set_m_scale(void * instance,void * field_value)
				{
					static_cast<Transform*>(instance)->m_scale = *static_cast<Vector3*>(field_value);
				}
				static void* get_m_scale(void * instance)
				{
					return &(static_cast<Transform*>(instance)->m_scale);
				}
				static const char* getFieldName_m_scale()
				{
					return "m_scale";
				}
				static const char* getFieldTypeName_m_scale()
				{
					return "Vector3";
				}
				static bool isArray_m_scale()
				{
					return false;
				}
				static void set_m_rotation(void * instance,void * field_value)
				{
					static_cast<Transform*>(instance)->m_rotation = *static_cast<Quaternion*>(field_value);
				}
				static void* get_m_rotation(void * instance)
				{
					return &(static_cast<Transform*>(instance)->m_rotation);
				}
				static const char* getFieldName_m_rotation()
				{
					return "m_rotation";
				}
				static const char* getFieldTypeName_m_rotation()
				{
					return "Quaternion";
				}
				static bool isArray_m_rotation()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_Transform()
		{
			FieldFunctionTuple * f_field_function_tuple_m_position = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeTransformOperator::set_m_position,
			&TypeFieldReflectionOperator::TypeTransformOperator::get_m_position,
			&TypeFieldReflectionOperator::TypeTransformOperator::getClassName,
			&TypeFieldReflectionOperator::TypeTransformOperator::getFieldName_m_position,
			&TypeFieldReflectionOperator::TypeTransformOperator::getFieldTypeName_m_position,
			&TypeFieldReflectionOperator::TypeTransformOperator::isArray_m_position
			);
			REGISTER_TO_FIELD_MAP("m_position",f_field_function_tuple_m_position);
			FieldFunctionTuple * f_field_function_tuple_m_scale = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeTransformOperator::set_m_scale,
			&TypeFieldReflectionOperator::TypeTransformOperator::get_m_scale,
			&TypeFieldReflectionOperator::TypeTransformOperator::getClassName,
			&TypeFieldReflectionOperator::TypeTransformOperator::getFieldName_m_scale,
			&TypeFieldReflectionOperator::TypeTransformOperator::getFieldTypeName_m_scale,
			&TypeFieldReflectionOperator::TypeTransformOperator::isArray_m_scale
			);
			REGISTER_TO_FIELD_MAP("m_scale",f_field_function_tuple_m_scale);
			FieldFunctionTuple * f_field_function_tuple_m_rotation = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeTransformOperator::set_m_rotation,
			&TypeFieldReflectionOperator::TypeTransformOperator::get_m_rotation,
			&TypeFieldReflectionOperator::TypeTransformOperator::getClassName,
			&TypeFieldReflectionOperator::TypeTransformOperator::getFieldName_m_rotation,
			&TypeFieldReflectionOperator::TypeTransformOperator::getFieldTypeName_m_rotation,
			&TypeFieldReflectionOperator::TypeTransformOperator::isArray_m_rotation
			);
			REGISTER_TO_FIELD_MAP("m_rotation",f_field_function_tuple_m_rotation);
		
		
		ClassFunctionTuple * f_class_function_tuple_Transform = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeTransformOperator::getTransformBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeTransformOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeTransformOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Transform",f_class_function_tuple_Transform);
		}
		namespace TypeWrappersRegister
		{
			void Transform(){TypeWrapperRegister_Transform();}
		}
		
	}
}

