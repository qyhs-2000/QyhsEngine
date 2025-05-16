#pragma once
#include "runtime\resource\type\data\camera_config.h"

namespace qyhs
{
	class CameraPose;
	class CameraConfig;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeCameraPoseOperator
			{
			public:
				//class
				//static void getCameraPoseBaseClassReflectionInstanceList()
				static const char* getClassName(){return "CameraPose";}
				static void* constructorWithJson(const Json & json_context)
				{
					CameraPose * ret_instance = new CameraPose;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getCameraPoseBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(CameraPose*)instance_ptr);
				}
				//field
				static void set_m_position(void * instance,void * field_value)
				{
					static_cast<CameraPose*>(instance)->m_position = *static_cast<Vector3*>(field_value);
				}
				static void* get_m_position(void * instance)
				{
					return &(static_cast<CameraPose*>(instance)->m_position);
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
				static void set_m_target(void * instance,void * field_value)
				{
					static_cast<CameraPose*>(instance)->m_target = *static_cast<Vector3*>(field_value);
				}
				static void* get_m_target(void * instance)
				{
					return &(static_cast<CameraPose*>(instance)->m_target);
				}
				static const char* getFieldName_m_target()
				{
					return "m_target";
				}
				static const char* getFieldTypeName_m_target()
				{
					return "Vector3";
				}
				static bool isArray_m_target()
				{
					return false;
				}
				static void set_m_up(void * instance,void * field_value)
				{
					static_cast<CameraPose*>(instance)->m_up = *static_cast<Vector3*>(field_value);
				}
				static void* get_m_up(void * instance)
				{
					return &(static_cast<CameraPose*>(instance)->m_up);
				}
				static const char* getFieldName_m_up()
				{
					return "m_up";
				}
				static const char* getFieldTypeName_m_up()
				{
					return "Vector3";
				}
				static bool isArray_m_up()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeCameraConfigOperator
			{
			public:
				//class
				//static void getCameraConfigBaseClassReflectionInstanceList()
				static const char* getClassName(){return "CameraConfig";}
				static void* constructorWithJson(const Json & json_context)
				{
					CameraConfig * ret_instance = new CameraConfig;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getCameraConfigBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(CameraConfig*)instance_ptr);
				}
				//field
				static void set_m_pose(void * instance,void * field_value)
				{
					static_cast<CameraConfig*>(instance)->m_pose = *static_cast<CameraPose*>(field_value);
				}
				static void* get_m_pose(void * instance)
				{
					return &(static_cast<CameraConfig*>(instance)->m_pose);
				}
				static const char* getFieldName_m_pose()
				{
					return "m_pose";
				}
				static const char* getFieldTypeName_m_pose()
				{
					return "CameraPose";
				}
				static bool isArray_m_pose()
				{
					return false;
				}
				static void set_m_aspect(void * instance,void * field_value)
				{
					static_cast<CameraConfig*>(instance)->m_aspect = *static_cast<Vector2*>(field_value);
				}
				static void* get_m_aspect(void * instance)
				{
					return &(static_cast<CameraConfig*>(instance)->m_aspect);
				}
				static const char* getFieldName_m_aspect()
				{
					return "m_aspect";
				}
				static const char* getFieldTypeName_m_aspect()
				{
					return "Vector2";
				}
				static bool isArray_m_aspect()
				{
					return false;
				}
				static void set_m_z_far(void * instance,void * field_value)
				{
					static_cast<CameraConfig*>(instance)->m_z_far = *static_cast<float*>(field_value);
				}
				static void* get_m_z_far(void * instance)
				{
					return &(static_cast<CameraConfig*>(instance)->m_z_far);
				}
				static const char* getFieldName_m_z_far()
				{
					return "m_z_far";
				}
				static const char* getFieldTypeName_m_z_far()
				{
					return "float";
				}
				static bool isArray_m_z_far()
				{
					return false;
				}
				static void set_m_z_near(void * instance,void * field_value)
				{
					static_cast<CameraConfig*>(instance)->m_z_near = *static_cast<float*>(field_value);
				}
				static void* get_m_z_near(void * instance)
				{
					return &(static_cast<CameraConfig*>(instance)->m_z_near);
				}
				static const char* getFieldName_m_z_near()
				{
					return "m_z_near";
				}
				static const char* getFieldTypeName_m_z_near()
				{
					return "float";
				}
				static bool isArray_m_z_near()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_CameraPose()
		{
			FieldFunctionTuple * f_field_function_tuple_m_position = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::set_m_position,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::get_m_position,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getFieldName_m_position,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getFieldTypeName_m_position,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::isArray_m_position
			);
			REGISTER_TO_FIELD_MAP("CameraPose",f_field_function_tuple_m_position);
			FieldFunctionTuple * f_field_function_tuple_m_target = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::set_m_target,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::get_m_target,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getFieldName_m_target,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getFieldTypeName_m_target,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::isArray_m_target
			);
			REGISTER_TO_FIELD_MAP("CameraPose",f_field_function_tuple_m_target);
			FieldFunctionTuple * f_field_function_tuple_m_up = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::set_m_up,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::get_m_up,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getFieldName_m_up,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::getFieldTypeName_m_up,
			&TypeFieldReflectionOperator::TypeCameraPoseOperator::isArray_m_up
			);
			REGISTER_TO_FIELD_MAP("CameraPose",f_field_function_tuple_m_up);
		
		
		ClassFunctionTuple * f_class_function_tuple_CameraPose = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeCameraPoseOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeCameraPoseOperator::getCameraPoseBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeCameraPoseOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("CameraPose",f_class_function_tuple_CameraPose);
		}
		void TypeWrapperRegister_CameraConfig()
		{
			FieldFunctionTuple * f_field_function_tuple_m_pose = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::set_m_pose,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::get_m_pose,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldName_m_pose,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldTypeName_m_pose,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::isArray_m_pose
			);
			REGISTER_TO_FIELD_MAP("CameraConfig",f_field_function_tuple_m_pose);
			FieldFunctionTuple * f_field_function_tuple_m_aspect = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::set_m_aspect,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::get_m_aspect,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldName_m_aspect,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldTypeName_m_aspect,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::isArray_m_aspect
			);
			REGISTER_TO_FIELD_MAP("CameraConfig",f_field_function_tuple_m_aspect);
			FieldFunctionTuple * f_field_function_tuple_m_z_far = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::set_m_z_far,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::get_m_z_far,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldName_m_z_far,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldTypeName_m_z_far,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::isArray_m_z_far
			);
			REGISTER_TO_FIELD_MAP("CameraConfig",f_field_function_tuple_m_z_far);
			FieldFunctionTuple * f_field_function_tuple_m_z_near = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::set_m_z_near,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::get_m_z_near,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldName_m_z_near,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::getFieldTypeName_m_z_near,
			&TypeFieldReflectionOperator::TypeCameraConfigOperator::isArray_m_z_near
			);
			REGISTER_TO_FIELD_MAP("CameraConfig",f_field_function_tuple_m_z_near);
		
		
		ClassFunctionTuple * f_class_function_tuple_CameraConfig = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeCameraConfigOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeCameraConfigOperator::getCameraConfigBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeCameraConfigOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("CameraConfig",f_class_function_tuple_CameraConfig);
		}
		namespace TypeWrappersRegister
		{
			void CameraPose(){TypeWrapperRegister_CameraPose();}
			void CameraConfig(){TypeWrapperRegister_CameraConfig();}
		}
		
	}
}

