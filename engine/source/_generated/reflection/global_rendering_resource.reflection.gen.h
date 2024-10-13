#pragma once
#include "runtime\resource\type\global_rendering_resource.h"

namespace QYHS
{
	class SkyBoxSpecularMap;
	class GlobalRenderConfig;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeSkyBoxSpecularMapOperator
			{
			public:
				//class
				//static void getSkyBoxSpecularMapBaseClassReflectionInstanceList()
				static const char* getClassName(){return "SkyBoxSpecularMap";}
				static void* constructorWithJson(const Json & json_context)
				{
					SkyBoxSpecularMap * ret_instance = new SkyBoxSpecularMap;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				//field
				static void set_m_positive_x_map(void * instance,void * field_value)
				{
					static_cast<SkyBoxSpecularMap*>(instance)->m_positive_x_map = *static_cast<std::string*>(field_value);
				}
				static void* get_m_positive_x_map(void * instance)
				{
					return &(static_cast<SkyBoxSpecularMap*>(instance)->m_positive_x_map);
				}
				static const char* getFieldName_m_positive_x_map()
				{
					return "m_positive_x_map";
				}
				static const char* getFieldTypeName_m_positive_x_map()
				{
					return "std::string";
				}
				static bool isArray_m_positive_x_map()
				{
					return false;
				}
				static void set_m_negative_x_map(void * instance,void * field_value)
				{
					static_cast<SkyBoxSpecularMap*>(instance)->m_negative_x_map = *static_cast<std::string*>(field_value);
				}
				static void* get_m_negative_x_map(void * instance)
				{
					return &(static_cast<SkyBoxSpecularMap*>(instance)->m_negative_x_map);
				}
				static const char* getFieldName_m_negative_x_map()
				{
					return "m_negative_x_map";
				}
				static const char* getFieldTypeName_m_negative_x_map()
				{
					return "std::string";
				}
				static bool isArray_m_negative_x_map()
				{
					return false;
				}
				static void set_m_positive_y_map(void * instance,void * field_value)
				{
					static_cast<SkyBoxSpecularMap*>(instance)->m_positive_y_map = *static_cast<std::string*>(field_value);
				}
				static void* get_m_positive_y_map(void * instance)
				{
					return &(static_cast<SkyBoxSpecularMap*>(instance)->m_positive_y_map);
				}
				static const char* getFieldName_m_positive_y_map()
				{
					return "m_positive_y_map";
				}
				static const char* getFieldTypeName_m_positive_y_map()
				{
					return "std::string";
				}
				static bool isArray_m_positive_y_map()
				{
					return false;
				}
				static void set_m_negative_y_map(void * instance,void * field_value)
				{
					static_cast<SkyBoxSpecularMap*>(instance)->m_negative_y_map = *static_cast<std::string*>(field_value);
				}
				static void* get_m_negative_y_map(void * instance)
				{
					return &(static_cast<SkyBoxSpecularMap*>(instance)->m_negative_y_map);
				}
				static const char* getFieldName_m_negative_y_map()
				{
					return "m_negative_y_map";
				}
				static const char* getFieldTypeName_m_negative_y_map()
				{
					return "std::string";
				}
				static bool isArray_m_negative_y_map()
				{
					return false;
				}
				static void set_m_positive_z_map(void * instance,void * field_value)
				{
					static_cast<SkyBoxSpecularMap*>(instance)->m_positive_z_map = *static_cast<std::string*>(field_value);
				}
				static void* get_m_positive_z_map(void * instance)
				{
					return &(static_cast<SkyBoxSpecularMap*>(instance)->m_positive_z_map);
				}
				static const char* getFieldName_m_positive_z_map()
				{
					return "m_positive_z_map";
				}
				static const char* getFieldTypeName_m_positive_z_map()
				{
					return "std::string";
				}
				static bool isArray_m_positive_z_map()
				{
					return false;
				}
				static void set_m_negative_z_map(void * instance,void * field_value)
				{
					static_cast<SkyBoxSpecularMap*>(instance)->m_negative_z_map = *static_cast<std::string*>(field_value);
				}
				static void* get_m_negative_z_map(void * instance)
				{
					return &(static_cast<SkyBoxSpecularMap*>(instance)->m_negative_z_map);
				}
				static const char* getFieldName_m_negative_z_map()
				{
					return "m_negative_z_map";
				}
				static const char* getFieldTypeName_m_negative_z_map()
				{
					return "std::string";
				}
				static bool isArray_m_negative_z_map()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
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
				static void set_m_skybox_specular_map(void * instance,void * field_value)
				{
					static_cast<GlobalRenderConfig*>(instance)->m_skybox_specular_map = *static_cast<SkyBoxSpecularMap*>(field_value);
				}
				static void* get_m_skybox_specular_map(void * instance)
				{
					return &(static_cast<GlobalRenderConfig*>(instance)->m_skybox_specular_map);
				}
				static const char* getFieldName_m_skybox_specular_map()
				{
					return "m_skybox_specular_map";
				}
				static const char* getFieldTypeName_m_skybox_specular_map()
				{
					return "SkyBoxSpecularMap";
				}
				static bool isArray_m_skybox_specular_map()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_SkyBoxSpecularMap()
		{
			FieldFunctionTuple * f_field_function_tuple_m_positive_x_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::set_m_positive_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::get_m_positive_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldName_m_positive_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldTypeName_m_positive_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::isArray_m_positive_x_map
			);
			REGISTER_TO_FIELD_MAP("m_positive_x_map",f_field_function_tuple_m_positive_x_map);
			FieldFunctionTuple * f_field_function_tuple_m_negative_x_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::set_m_negative_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::get_m_negative_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldName_m_negative_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldTypeName_m_negative_x_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::isArray_m_negative_x_map
			);
			REGISTER_TO_FIELD_MAP("m_negative_x_map",f_field_function_tuple_m_negative_x_map);
			FieldFunctionTuple * f_field_function_tuple_m_positive_y_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::set_m_positive_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::get_m_positive_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldName_m_positive_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldTypeName_m_positive_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::isArray_m_positive_y_map
			);
			REGISTER_TO_FIELD_MAP("m_positive_y_map",f_field_function_tuple_m_positive_y_map);
			FieldFunctionTuple * f_field_function_tuple_m_negative_y_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::set_m_negative_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::get_m_negative_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldName_m_negative_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldTypeName_m_negative_y_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::isArray_m_negative_y_map
			);
			REGISTER_TO_FIELD_MAP("m_negative_y_map",f_field_function_tuple_m_negative_y_map);
			FieldFunctionTuple * f_field_function_tuple_m_positive_z_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::set_m_positive_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::get_m_positive_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldName_m_positive_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldTypeName_m_positive_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::isArray_m_positive_z_map
			);
			REGISTER_TO_FIELD_MAP("m_positive_z_map",f_field_function_tuple_m_positive_z_map);
			FieldFunctionTuple * f_field_function_tuple_m_negative_z_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::set_m_negative_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::get_m_negative_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldName_m_negative_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getFieldTypeName_m_negative_z_map,
			&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::isArray_m_negative_z_map
			);
			REGISTER_TO_FIELD_MAP("m_negative_z_map",f_field_function_tuple_m_negative_z_map);
		
		
		ClassFunctionTuple * f_class_function_tuple_SkyBoxSpecularMap = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::getSkyBoxSpecularMapBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeSkyBoxSpecularMapOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("SkyBoxSpecularMap",f_class_function_tuple_SkyBoxSpecularMap);
		}
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
			FieldFunctionTuple * f_field_function_tuple_m_skybox_specular_map = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::set_m_skybox_specular_map,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::get_m_skybox_specular_map,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getClassName,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getFieldName_m_skybox_specular_map,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getFieldTypeName_m_skybox_specular_map,
			&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::isArray_m_skybox_specular_map
			);
			REGISTER_TO_FIELD_MAP("m_skybox_specular_map",f_field_function_tuple_m_skybox_specular_map);
		
		
		ClassFunctionTuple * f_class_function_tuple_GlobalRenderConfig = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::getGlobalRenderConfigBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeGlobalRenderConfigOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("GlobalRenderConfig",f_class_function_tuple_GlobalRenderConfig);
		}
		namespace TypeWrappersRegister
		{
			void SkyBoxSpecularMap(){TypeWrapperRegister_SkyBoxSpecularMap();}
			void GlobalRenderConfig(){TypeWrapperRegister_GlobalRenderConfig();}
		}
		
	}
}

