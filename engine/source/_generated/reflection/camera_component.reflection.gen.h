#pragma once
#include "runtime\function\framework\component\camera\camera_component.h"

namespace QYHS
{
	class CameraParamter;
	class ThirdPersonCameraParameter;
	class CameraComponentResource;
	class CameraComponent;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeCameraParamterOperator
			{
			public:
				//class
				//static void getCameraParamterBaseClassReflectionInstanceList()
				static const char* getClassName(){return "CameraParamter";}
				static void* constructorWithJson(const Json & json_context)
				{
					CameraParamter * ret_instance = new CameraParamter;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getCameraParamterBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(CameraParamter*)instance_ptr);
				}
				//field
				static void set_m_fov(void * instance,void * field_value)
				{
					static_cast<CameraParamter*>(instance)->m_fov = *static_cast<float*>(field_value);
				}
				static void* get_m_fov(void * instance)
				{
					return &(static_cast<CameraParamter*>(instance)->m_fov);
				}
				static const char* getFieldName_m_fov()
				{
					return "m_fov";
				}
				static const char* getFieldTypeName_m_fov()
				{
					return "float";
				}
				static bool isArray_m_fov()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeThirdPersonCameraParameterOperator
			{
			public:
				//class
				//static void getThirdPersonCameraParameterBaseClassReflectionInstanceList()
				static const char* getClassName(){return "ThirdPersonCameraParameter";}
				static void* constructorWithJson(const Json & json_context)
				{
					ThirdPersonCameraParameter * ret_instance = new ThirdPersonCameraParameter;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getThirdPersonCameraParameterBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(QYHS::CameraParamter,static_cast<ThirdPersonCameraParameter*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(ThirdPersonCameraParameter*)instance_ptr);
				}
				//field
				static void set_m_horizontal_offset(void * instance,void * field_value)
				{
					static_cast<ThirdPersonCameraParameter*>(instance)->m_horizontal_offset = *static_cast<float*>(field_value);
				}
				static void* get_m_horizontal_offset(void * instance)
				{
					return &(static_cast<ThirdPersonCameraParameter*>(instance)->m_horizontal_offset);
				}
				static const char* getFieldName_m_horizontal_offset()
				{
					return "m_horizontal_offset";
				}
				static const char* getFieldTypeName_m_horizontal_offset()
				{
					return "float";
				}
				static bool isArray_m_horizontal_offset()
				{
					return false;
				}
				static void set_m_vertical_offset(void * instance,void * field_value)
				{
					static_cast<ThirdPersonCameraParameter*>(instance)->m_vertical_offset = *static_cast<float*>(field_value);
				}
				static void* get_m_vertical_offset(void * instance)
				{
					return &(static_cast<ThirdPersonCameraParameter*>(instance)->m_vertical_offset);
				}
				static const char* getFieldName_m_vertical_offset()
				{
					return "m_vertical_offset";
				}
				static const char* getFieldTypeName_m_vertical_offset()
				{
					return "float";
				}
				static bool isArray_m_vertical_offset()
				{
					return false;
				}
				static void set_m_cursor_pitch(void * instance,void * field_value)
				{
					static_cast<ThirdPersonCameraParameter*>(instance)->m_cursor_pitch = *static_cast<Quaternion*>(field_value);
				}
				static void* get_m_cursor_pitch(void * instance)
				{
					return &(static_cast<ThirdPersonCameraParameter*>(instance)->m_cursor_pitch);
				}
				static const char* getFieldName_m_cursor_pitch()
				{
					return "m_cursor_pitch";
				}
				static const char* getFieldTypeName_m_cursor_pitch()
				{
					return "Quaternion";
				}
				static bool isArray_m_cursor_pitch()
				{
					return false;
				}
				static void set_m_cursor_yaw(void * instance,void * field_value)
				{
					static_cast<ThirdPersonCameraParameter*>(instance)->m_cursor_yaw = *static_cast<Quaternion*>(field_value);
				}
				static void* get_m_cursor_yaw(void * instance)
				{
					return &(static_cast<ThirdPersonCameraParameter*>(instance)->m_cursor_yaw);
				}
				static const char* getFieldName_m_cursor_yaw()
				{
					return "m_cursor_yaw";
				}
				static const char* getFieldTypeName_m_cursor_yaw()
				{
					return "Quaternion";
				}
				static bool isArray_m_cursor_yaw()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeCameraComponentResourceOperator
			{
			public:
				//class
				//static void getCameraComponentResourceBaseClassReflectionInstanceList()
				static const char* getClassName(){return "CameraComponentResource";}
				static void* constructorWithJson(const Json & json_context)
				{
					CameraComponentResource * ret_instance = new CameraComponentResource;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getCameraComponentResourceBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(CameraComponentResource*)instance_ptr);
				}
				//field
				static void set_m_parameter(void * instance,void * field_value)
				{
					static_cast<CameraComponentResource*>(instance)->m_parameter = *static_cast<Reflection::ReflectionPtr<CameraParamter>*>(field_value);
				}
				static void* get_m_parameter(void * instance)
				{
					return &(static_cast<CameraComponentResource*>(instance)->m_parameter);
				}
				static const char* getFieldName_m_parameter()
				{
					return "m_parameter";
				}
				static const char* getFieldTypeName_m_parameter()
				{
					return "Reflection::ReflectionPtr<CameraParamter>";
				}
				static bool isArray_m_parameter()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeCameraComponentOperator
			{
			public:
				//class
				//static void getCameraComponentBaseClassReflectionInstanceList()
				static const char* getClassName(){return "CameraComponent";}
				static void* constructorWithJson(const Json & json_context)
				{
					CameraComponent * ret_instance = new CameraComponent;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getCameraComponentBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 1;
					out_list = new ReflectionInstance[count];
					for(int i = 0;i<count;++i){
						out_list[i] = TYPE_META_DEF(QYHS::Component,static_cast<CameraComponent*>(instance));
					}
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(CameraComponent*)instance_ptr);
				}
				//field
				static void set_m_camera_res(void * instance,void * field_value)
				{
					static_cast<CameraComponent*>(instance)->m_camera_res = *static_cast<CameraComponentResource*>(field_value);
				}
				static void* get_m_camera_res(void * instance)
				{
					return &(static_cast<CameraComponent*>(instance)->m_camera_res);
				}
				static const char* getFieldName_m_camera_res()
				{
					return "m_camera_res";
				}
				static const char* getFieldTypeName_m_camera_res()
				{
					return "CameraComponentResource";
				}
				static bool isArray_m_camera_res()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		

		void TypeWrapperRegister_CameraParamter()
		{
			FieldFunctionTuple * f_field_function_tuple_m_fov = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraParamterOperator::set_m_fov,
			&TypeFieldReflectionOperator::TypeCameraParamterOperator::get_m_fov,
			&TypeFieldReflectionOperator::TypeCameraParamterOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraParamterOperator::getFieldName_m_fov,
			&TypeFieldReflectionOperator::TypeCameraParamterOperator::getFieldTypeName_m_fov,
			&TypeFieldReflectionOperator::TypeCameraParamterOperator::isArray_m_fov
			);
			REGISTER_TO_FIELD_MAP("CameraParamter",f_field_function_tuple_m_fov);
		
		
		ClassFunctionTuple * f_class_function_tuple_CameraParamter = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeCameraParamterOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeCameraParamterOperator::getCameraParamterBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeCameraParamterOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("CameraParamter",f_class_function_tuple_CameraParamter);
		}
		void TypeWrapperRegister_ThirdPersonCameraParameter()
		{
			FieldFunctionTuple * f_field_function_tuple_m_horizontal_offset = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::set_m_horizontal_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::get_m_horizontal_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getClassName,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldName_m_horizontal_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldTypeName_m_horizontal_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::isArray_m_horizontal_offset
			);
			REGISTER_TO_FIELD_MAP("ThirdPersonCameraParameter",f_field_function_tuple_m_horizontal_offset);
			FieldFunctionTuple * f_field_function_tuple_m_vertical_offset = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::set_m_vertical_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::get_m_vertical_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getClassName,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldName_m_vertical_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldTypeName_m_vertical_offset,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::isArray_m_vertical_offset
			);
			REGISTER_TO_FIELD_MAP("ThirdPersonCameraParameter",f_field_function_tuple_m_vertical_offset);
			FieldFunctionTuple * f_field_function_tuple_m_cursor_pitch = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::set_m_cursor_pitch,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::get_m_cursor_pitch,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getClassName,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldName_m_cursor_pitch,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldTypeName_m_cursor_pitch,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::isArray_m_cursor_pitch
			);
			REGISTER_TO_FIELD_MAP("ThirdPersonCameraParameter",f_field_function_tuple_m_cursor_pitch);
			FieldFunctionTuple * f_field_function_tuple_m_cursor_yaw = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::set_m_cursor_yaw,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::get_m_cursor_yaw,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getClassName,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldName_m_cursor_yaw,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getFieldTypeName_m_cursor_yaw,
			&TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::isArray_m_cursor_yaw
			);
			REGISTER_TO_FIELD_MAP("ThirdPersonCameraParameter",f_field_function_tuple_m_cursor_yaw);
		
		
		ClassFunctionTuple * f_class_function_tuple_ThirdPersonCameraParameter = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::getThirdPersonCameraParameterBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeThirdPersonCameraParameterOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("ThirdPersonCameraParameter",f_class_function_tuple_ThirdPersonCameraParameter);
		}
		void TypeWrapperRegister_CameraComponentResource()
		{
			FieldFunctionTuple * f_field_function_tuple_m_parameter = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::set_m_parameter,
			&TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::get_m_parameter,
			&TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::getFieldName_m_parameter,
			&TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::getFieldTypeName_m_parameter,
			&TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::isArray_m_parameter
			);
			REGISTER_TO_FIELD_MAP("CameraComponentResource",f_field_function_tuple_m_parameter);
		
		
		ClassFunctionTuple * f_class_function_tuple_CameraComponentResource = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::getCameraComponentResourceBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeCameraComponentResourceOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("CameraComponentResource",f_class_function_tuple_CameraComponentResource);
		}
		void TypeWrapperRegister_CameraComponent()
		{
			FieldFunctionTuple * f_field_function_tuple_m_camera_res = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeCameraComponentOperator::set_m_camera_res,
			&TypeFieldReflectionOperator::TypeCameraComponentOperator::get_m_camera_res,
			&TypeFieldReflectionOperator::TypeCameraComponentOperator::getClassName,
			&TypeFieldReflectionOperator::TypeCameraComponentOperator::getFieldName_m_camera_res,
			&TypeFieldReflectionOperator::TypeCameraComponentOperator::getFieldTypeName_m_camera_res,
			&TypeFieldReflectionOperator::TypeCameraComponentOperator::isArray_m_camera_res
			);
			REGISTER_TO_FIELD_MAP("CameraComponent",f_field_function_tuple_m_camera_res);
		
		
		ClassFunctionTuple * f_class_function_tuple_CameraComponent = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeCameraComponentOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeCameraComponentOperator::getCameraComponentBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeCameraComponentOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("CameraComponent",f_class_function_tuple_CameraComponent);
		}
		namespace TypeWrappersRegister
		{
			void CameraParamter(){TypeWrapperRegister_CameraParamter();}
			void ThirdPersonCameraParameter(){TypeWrapperRegister_ThirdPersonCameraParameter();}
			void CameraComponentResource(){TypeWrapperRegister_CameraComponentResource();}
			void CameraComponent(){TypeWrapperRegister_CameraComponent();}
		}
		
	}
}

