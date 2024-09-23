#pragma once
#include "runtime\resource\type\global_rendering_resource.h"

namespace QYHS
{
	class GlobalRenderConfig;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeGlobalRenderConfigOperator
			{
			public:
				//class
				//static void getGlobalRenderConfigBaseClassReflectionInstanceList()
				static const char* getClassName(){return "GlobalRenderConfig";}
				static void* constructorWithJson(const Json & json_context)
				{
					GlobalRenderConfig * ret_instance = new GlobalRenderConfig;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_camera_config(void * instance,void * field_value)
				{
					static_cast<GlobalRenderConfig*>(instance)->camera_config = *static_cast<CameraConfig*>(field_value);
				}
				static void* get_camera_config(void * instance)
				{
					return &(static_cast<GlobalRenderConfig*>(instance)->camera_config);
				}
				static const char* getFieldName_camera_config()
				{
					return "camera_config";
				}
				static const char* getFieldTypeName_camera_config()
				{
					return "CameraConfig";
				}
				static bool isArray_camera_config()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_GlobalRenderConfig()
		{
			FieldFunctionTuple * f_field_function_tuple_camera_config = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::set_camera_config,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::get_camera_config,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getClassName,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getFieldName_camera_config,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getFieldTypeName_camera_config,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::isArray_camera_config
			);
			REGISTER_TO_FIELD_MAP("camera_config",f_field_function_tuple_camera_config);
		
		
		ClassFunctionTuple * f_class_function_tuple_GlobalRenderConfig = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getGlobalRenderConfigBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("GlobalRenderConfig",f_class_function_tuple_GlobalRenderConfig);
		}
		namespace TypeWrappersRegister
		{
			void GlobalRenderConfig(){TypeWrapperRegister_GlobalRenderConfig();}
		}
		
	}
}

