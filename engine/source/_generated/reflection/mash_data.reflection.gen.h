#pragma once
#include "runtime\resource\type\data\mash_data.h"

namespace QYHS
{
	class Vertex;
	class SkeletonBind;
	class MeshData;
	namespace Reflection
	{
		namespace TypeFieldReflectionOperator
		{
			class TypeVertexOperator
			{
			public:
				//class
				//static void getVertexBaseClassReflectionInstanceList()
				static const char* getClassName(){return "Vertex";}
				static void* constructorWithJson(const Json & json_context)
				{
					Vertex * ret_instance = new Vertex;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getVertexBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(Vertex*)instance_ptr);
				}
				//field
				static void set_px(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->px = *static_cast<float*>(field_value);
				}
				static void* get_px(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->px);
				}
				static const char* getFieldName_px()
				{
					return "px";
				}
				static const char* getFieldTypeName_px()
				{
					return "float";
				}
				static bool isArray_px()
				{
					return false;
				}
				static void set_py(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->py = *static_cast<float*>(field_value);
				}
				static void* get_py(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->py);
				}
				static const char* getFieldName_py()
				{
					return "py";
				}
				static const char* getFieldTypeName_py()
				{
					return "float";
				}
				static bool isArray_py()
				{
					return false;
				}
				static void set_pz(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->pz = *static_cast<float*>(field_value);
				}
				static void* get_pz(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->pz);
				}
				static const char* getFieldName_pz()
				{
					return "pz";
				}
				static const char* getFieldTypeName_pz()
				{
					return "float";
				}
				static bool isArray_pz()
				{
					return false;
				}
				static void set_nx(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->nx = *static_cast<float*>(field_value);
				}
				static void* get_nx(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->nx);
				}
				static const char* getFieldName_nx()
				{
					return "nx";
				}
				static const char* getFieldTypeName_nx()
				{
					return "float";
				}
				static bool isArray_nx()
				{
					return false;
				}
				static void set_ny(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->ny = *static_cast<float*>(field_value);
				}
				static void* get_ny(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->ny);
				}
				static const char* getFieldName_ny()
				{
					return "ny";
				}
				static const char* getFieldTypeName_ny()
				{
					return "float";
				}
				static bool isArray_ny()
				{
					return false;
				}
				static void set_nz(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->nz = *static_cast<float*>(field_value);
				}
				static void* get_nz(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->nz);
				}
				static const char* getFieldName_nz()
				{
					return "nz";
				}
				static const char* getFieldTypeName_nz()
				{
					return "float";
				}
				static bool isArray_nz()
				{
					return false;
				}
				static void set_tx(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->tx = *static_cast<float*>(field_value);
				}
				static void* get_tx(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->tx);
				}
				static const char* getFieldName_tx()
				{
					return "tx";
				}
				static const char* getFieldTypeName_tx()
				{
					return "float";
				}
				static bool isArray_tx()
				{
					return false;
				}
				static void set_ty(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->ty = *static_cast<float*>(field_value);
				}
				static void* get_ty(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->ty);
				}
				static const char* getFieldName_ty()
				{
					return "ty";
				}
				static const char* getFieldTypeName_ty()
				{
					return "float";
				}
				static bool isArray_ty()
				{
					return false;
				}
				static void set_tz(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->tz = *static_cast<float*>(field_value);
				}
				static void* get_tz(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->tz);
				}
				static const char* getFieldName_tz()
				{
					return "tz";
				}
				static const char* getFieldTypeName_tz()
				{
					return "float";
				}
				static bool isArray_tz()
				{
					return false;
				}
				static void set_u(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->u = *static_cast<float*>(field_value);
				}
				static void* get_u(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->u);
				}
				static const char* getFieldName_u()
				{
					return "u";
				}
				static const char* getFieldTypeName_u()
				{
					return "float";
				}
				static bool isArray_u()
				{
					return false;
				}
				static void set_v(void * instance,void * field_value)
				{
					static_cast<Vertex*>(instance)->v = *static_cast<float*>(field_value);
				}
				static void* get_v(void * instance)
				{
					return &(static_cast<Vertex*>(instance)->v);
				}
				static const char* getFieldName_v()
				{
					return "v";
				}
				static const char* getFieldTypeName_v()
				{
					return "float";
				}
				static bool isArray_v()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeSkeletonBindOperator
			{
			public:
				//class
				//static void getSkeletonBindBaseClassReflectionInstanceList()
				static const char* getClassName(){return "SkeletonBind";}
				static void* constructorWithJson(const Json & json_context)
				{
					SkeletonBind * ret_instance = new SkeletonBind;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getSkeletonBindBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(SkeletonBind*)instance_ptr);
				}
				//field
				static void set_index0(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->index0 = *static_cast<int*>(field_value);
				}
				static void* get_index0(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->index0);
				}
				static const char* getFieldName_index0()
				{
					return "index0";
				}
				static const char* getFieldTypeName_index0()
				{
					return "int";
				}
				static bool isArray_index0()
				{
					return false;
				}
				static void set_index1(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->index1 = *static_cast<int*>(field_value);
				}
				static void* get_index1(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->index1);
				}
				static const char* getFieldName_index1()
				{
					return "index1";
				}
				static const char* getFieldTypeName_index1()
				{
					return "int";
				}
				static bool isArray_index1()
				{
					return false;
				}
				static void set_index2(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->index2 = *static_cast<int*>(field_value);
				}
				static void* get_index2(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->index2);
				}
				static const char* getFieldName_index2()
				{
					return "index2";
				}
				static const char* getFieldTypeName_index2()
				{
					return "int";
				}
				static bool isArray_index2()
				{
					return false;
				}
				static void set_index3(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->index3 = *static_cast<int*>(field_value);
				}
				static void* get_index3(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->index3);
				}
				static const char* getFieldName_index3()
				{
					return "index3";
				}
				static const char* getFieldTypeName_index3()
				{
					return "int";
				}
				static bool isArray_index3()
				{
					return false;
				}
				static void set_weight0(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->weight0 = *static_cast<float*>(field_value);
				}
				static void* get_weight0(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->weight0);
				}
				static const char* getFieldName_weight0()
				{
					return "weight0";
				}
				static const char* getFieldTypeName_weight0()
				{
					return "float";
				}
				static bool isArray_weight0()
				{
					return false;
				}
				static void set_weight1(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->weight1 = *static_cast<float*>(field_value);
				}
				static void* get_weight1(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->weight1);
				}
				static const char* getFieldName_weight1()
				{
					return "weight1";
				}
				static const char* getFieldTypeName_weight1()
				{
					return "float";
				}
				static bool isArray_weight1()
				{
					return false;
				}
				static void set_weight2(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->weight2 = *static_cast<float*>(field_value);
				}
				static void* get_weight2(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->weight2);
				}
				static const char* getFieldName_weight2()
				{
					return "weight2";
				}
				static const char* getFieldTypeName_weight2()
				{
					return "float";
				}
				static bool isArray_weight2()
				{
					return false;
				}
				static void set_weight3(void * instance,void * field_value)
				{
					static_cast<SkeletonBind*>(instance)->weight3 = *static_cast<float*>(field_value);
				}
				static void* get_weight3(void * instance)
				{
					return &(static_cast<SkeletonBind*>(instance)->weight3);
				}
				static const char* getFieldName_weight3()
				{
					return "weight3";
				}
				static const char* getFieldTypeName_weight3()
				{
					return "float";
				}
				static bool isArray_weight3()
				{
					return false;
				}
			};
		}//namespace TypeFieldFunctionOperator

		
		namespace TypeFieldReflectionOperator
		{
			class TypeMeshDataOperator
			{
			public:
				//class
				//static void getMeshDataBaseClassReflectionInstanceList()
				static const char* getClassName(){return "MeshData";}
				static void* constructorWithJson(const Json & json_context)
				{
					MeshData * ret_instance = new MeshData;
					Serializer::read(json_context,*ret_instance);
					return ret_instance;
				}
				static int getMeshDataBaseClassReflectionInstanceList(ReflectionInstance* &out_list, void* instance)
				{
					int count = 0;
					
					return count;	
				}
				static Json writeByName(void * instance_ptr)
				{
					return Serializer::write(*(MeshData*)instance_ptr);
				}
				//field
				static void set_vertex_buffer(void * instance,void * field_value)
				{
					static_cast<MeshData*>(instance)->vertex_buffer = *static_cast<std::vector<Vertex>*>(field_value);
				}
				static void* get_vertex_buffer(void * instance)
				{
					return &(static_cast<MeshData*>(instance)->vertex_buffer);
				}
				static const char* getFieldName_vertex_buffer()
				{
					return "vertex_buffer";
				}
				static const char* getFieldTypeName_vertex_buffer()
				{
					return "std::vector<Vertex>";
				}
				static bool isArray_vertex_buffer()
				{
					return true;
				}
				static void set_index_buffer(void * instance,void * field_value)
				{
					static_cast<MeshData*>(instance)->index_buffer = *static_cast<std::vector<uint16_t>*>(field_value);
				}
				static void* get_index_buffer(void * instance)
				{
					return &(static_cast<MeshData*>(instance)->index_buffer);
				}
				static const char* getFieldName_index_buffer()
				{
					return "index_buffer";
				}
				static const char* getFieldTypeName_index_buffer()
				{
					return "std::vector<uint16_t>";
				}
				static bool isArray_index_buffer()
				{
					return true;
				}
				static void set_bind(void * instance,void * field_value)
				{
					static_cast<MeshData*>(instance)->bind = *static_cast<std::vector<SkeletonBind>*>(field_value);
				}
				static void* get_bind(void * instance)
				{
					return &(static_cast<MeshData*>(instance)->bind);
				}
				static const char* getFieldName_bind()
				{
					return "bind";
				}
				static const char* getFieldTypeName_bind()
				{
					return "std::vector<SkeletonBind>";
				}
				static bool isArray_bind()
				{
					return true;
				}
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
		#ifndef ArraystdSSvectorLSkeletonBindROperatorMacro
		#define ArraystdSSvectorLSkeletonBindROperatorMacro
		class ArraystdSSvectorLSkeletonBindROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<SkeletonBind>";}
			static const char* getElementTypeName(){return "SkeletonBind";}
			static int getSize(void * instance){return (static_cast<std::vector<SkeletonBind>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<SkeletonBind>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<SkeletonBind>*>(instance))[index] = *(static_cast<SkeletonBind*>(value));
			}
		};
		#endif
		#ifndef ArraystdSSvectorLVertexROperatorMacro
		#define ArraystdSSvectorLVertexROperatorMacro
		class ArraystdSSvectorLVertexROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<Vertex>";}
			static const char* getElementTypeName(){return "Vertex";}
			static int getSize(void * instance){return (static_cast<std::vector<Vertex>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<Vertex>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<Vertex>*>(instance))[index] = *(static_cast<Vertex*>(value));
			}
		};
		#endif
		#ifndef ArraystdSSvectorLuint16_tROperatorMacro
		#define ArraystdSSvectorLuint16_tROperatorMacro
		class ArraystdSSvectorLuint16_tROperator
		{
		public:
			static const char* getArrayTypeName(){return "std::vector<uint16_t>";}
			static const char* getElementTypeName(){return "uint16_t";}
			static int getSize(void * instance){return (static_cast<std::vector<uint16_t>*>(instance)->size());}
			static void* get(int index,void * instance)
			{
				return static_cast<void*>(&((*static_cast<std::vector<uint16_t>*>(instance))[index]));
			}
			static void set(int index,void * instance,void * value)
			{
				(*static_cast<std::vector<uint16_t>*>(instance))[index] = *(static_cast<uint16_t*>(value));
			}
		};
		#endif
		}//namespace ArrayFunctioinOperator

		void TypeWrapperRegister_Vertex()
		{
			FieldFunctionTuple * f_field_function_tuple_px = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_px,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_px,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_px,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_px,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_px
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_px);
			FieldFunctionTuple * f_field_function_tuple_py = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_py
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_py);
			FieldFunctionTuple * f_field_function_tuple_pz = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_pz
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_pz);
			FieldFunctionTuple * f_field_function_tuple_nx = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_nx
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_nx);
			FieldFunctionTuple * f_field_function_tuple_ny = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_ny
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_ny);
			FieldFunctionTuple * f_field_function_tuple_nz = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_nz
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_nz);
			FieldFunctionTuple * f_field_function_tuple_tx = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_tx
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_tx);
			FieldFunctionTuple * f_field_function_tuple_ty = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_ty
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_ty);
			FieldFunctionTuple * f_field_function_tuple_tz = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_tz
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_tz);
			FieldFunctionTuple * f_field_function_tuple_u = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_u
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_u);
			FieldFunctionTuple * f_field_function_tuple_v = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_v
			);
			REGISTER_TO_FIELD_MAP("Vertex",f_field_function_tuple_v);
		
		
		ClassFunctionTuple * f_class_function_tuple_Vertex = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeVertexOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeVertexOperator::getVertexBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeVertexOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Vertex",f_class_function_tuple_Vertex);
		}
		void TypeWrapperRegister_SkeletonBind()
		{
			FieldFunctionTuple * f_field_function_tuple_index0 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_index0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_index0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_index0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_index0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_index0
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_index0);
			FieldFunctionTuple * f_field_function_tuple_index1 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_index1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_index1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_index1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_index1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_index1
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_index1);
			FieldFunctionTuple * f_field_function_tuple_index2 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_index2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_index2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_index2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_index2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_index2
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_index2);
			FieldFunctionTuple * f_field_function_tuple_index3 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_index3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_index3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_index3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_index3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_index3
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_index3);
			FieldFunctionTuple * f_field_function_tuple_weight0 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_weight0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_weight0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_weight0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_weight0,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_weight0
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_weight0);
			FieldFunctionTuple * f_field_function_tuple_weight1 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_weight1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_weight1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_weight1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_weight1,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_weight1
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_weight1);
			FieldFunctionTuple * f_field_function_tuple_weight2 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_weight2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_weight2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_weight2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_weight2,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_weight2
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_weight2);
			FieldFunctionTuple * f_field_function_tuple_weight3 = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::set_weight3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::get_weight3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getClassName,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldName_weight3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::getFieldTypeName_weight3,
			&TypeFieldReflectionOperator::TypeSkeletonBindOperator::isArray_weight3
			);
			REGISTER_TO_FIELD_MAP("SkeletonBind",f_field_function_tuple_weight3);
		
		
		ClassFunctionTuple * f_class_function_tuple_SkeletonBind = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeSkeletonBindOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeSkeletonBindOperator::getSkeletonBindBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeSkeletonBindOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("SkeletonBind",f_class_function_tuple_SkeletonBind);
		}
		void TypeWrapperRegister_MeshData()
		{
			FieldFunctionTuple * f_field_function_tuple_vertex_buffer = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMeshDataOperator::set_vertex_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::get_vertex_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldName_vertex_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldTypeName_vertex_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::isArray_vertex_buffer
			);
			REGISTER_TO_FIELD_MAP("MeshData",f_field_function_tuple_vertex_buffer);
			FieldFunctionTuple * f_field_function_tuple_index_buffer = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMeshDataOperator::set_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::get_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldName_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldTypeName_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::isArray_index_buffer
			);
			REGISTER_TO_FIELD_MAP("MeshData",f_field_function_tuple_index_buffer);
			FieldFunctionTuple * f_field_function_tuple_bind = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMeshDataOperator::set_bind,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::get_bind,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldName_bind,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldTypeName_bind,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::isArray_bind
			);
			REGISTER_TO_FIELD_MAP("MeshData",f_field_function_tuple_bind);
		
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLSkeletonBindR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLSkeletonBindROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLSkeletonBindROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLSkeletonBindROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLSkeletonBindROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLSkeletonBindROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLSkeletonBindR",array_function_tuple_stdSSvectorLSkeletonBindR);
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLVertexR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLVertexROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLVertexROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLVertexROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLVertexROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLVertexROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLVertexR",array_function_tuple_stdSSvectorLVertexR);
			ArrayFunctionTuple * array_function_tuple_stdSSvectorLuint16_tR = new ArrayFunctionTuple(
			&ArrayReflectionOperator::ArraystdSSvectorLuint16_tROperator::set,
			&ArrayReflectionOperator::ArraystdSSvectorLuint16_tROperator::get,
			&ArrayReflectionOperator::ArraystdSSvectorLuint16_tROperator::getSize,
			&ArrayReflectionOperator::ArraystdSSvectorLuint16_tROperator::getArrayTypeName,
			&ArrayReflectionOperator::ArraystdSSvectorLuint16_tROperator::getElementTypeName
		);
		REGISTER_TO_ARRAY_MAP("stdSSvectorLuint16_tR",array_function_tuple_stdSSvectorLuint16_tR);
		ClassFunctionTuple * f_class_function_tuple_MeshData = new ClassFunctionTuple(
            &TypeFieldReflectionOperator::TypeMeshDataOperator::constructorWithJson,
            &TypeFieldReflectionOperator::TypeMeshDataOperator::getMeshDataBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMeshDataOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MeshData",f_class_function_tuple_MeshData);
		}
		namespace TypeWrappersRegister
		{
			void Vertex(){TypeWrapperRegister_Vertex();}
			void SkeletonBind(){TypeWrapperRegister_SkeletonBind();}
			void MeshData(){TypeWrapperRegister_MeshData();}
		}
		
	}
}

