#pragma once
#include "runtime\resource\type\data\mash_data.h"

namespace QYHS
{
	class Vertex;
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
			};
		}//namespace TypeFieldFunctionOperator

		namespace ArrayReflectionOperator
		{
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
			REGISTER_TO_FIELD_MAP("px",f_field_function_tuple_px);
			FieldFunctionTuple * f_field_function_tuple_py = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_py,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_py
			);
			REGISTER_TO_FIELD_MAP("py",f_field_function_tuple_py);
			FieldFunctionTuple * f_field_function_tuple_pz = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_pz,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_pz
			);
			REGISTER_TO_FIELD_MAP("pz",f_field_function_tuple_pz);
			FieldFunctionTuple * f_field_function_tuple_nx = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_nx,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_nx
			);
			REGISTER_TO_FIELD_MAP("nx",f_field_function_tuple_nx);
			FieldFunctionTuple * f_field_function_tuple_ny = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_ny,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_ny
			);
			REGISTER_TO_FIELD_MAP("ny",f_field_function_tuple_ny);
			FieldFunctionTuple * f_field_function_tuple_nz = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_nz,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_nz
			);
			REGISTER_TO_FIELD_MAP("nz",f_field_function_tuple_nz);
			FieldFunctionTuple * f_field_function_tuple_tx = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_tx,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_tx
			);
			REGISTER_TO_FIELD_MAP("tx",f_field_function_tuple_tx);
			FieldFunctionTuple * f_field_function_tuple_ty = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_ty,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_ty
			);
			REGISTER_TO_FIELD_MAP("ty",f_field_function_tuple_ty);
			FieldFunctionTuple * f_field_function_tuple_tz = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_tz,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_tz
			);
			REGISTER_TO_FIELD_MAP("tz",f_field_function_tuple_tz);
			FieldFunctionTuple * f_field_function_tuple_u = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_u,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_u
			);
			REGISTER_TO_FIELD_MAP("u",f_field_function_tuple_u);
			FieldFunctionTuple * f_field_function_tuple_v = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeVertexOperator::set_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::get_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::getClassName,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldName_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::getFieldTypeName_v,
			&TypeFieldReflectionOperator::TypeVertexOperator::isArray_v
			);
			REGISTER_TO_FIELD_MAP("v",f_field_function_tuple_v);
		
		
		ClassFunctionTuple * f_class_function_tuple_Vertex = new ClassFunctionTuple(
			//&TypeFieldReflectionOperator::TypeVertexOperator::getVertexBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeVertexOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeVertexOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("Vertex",f_class_function_tuple_Vertex);
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
			REGISTER_TO_FIELD_MAP("vertex_buffer",f_field_function_tuple_vertex_buffer);
			FieldFunctionTuple * f_field_function_tuple_index_buffer = new FieldFunctionTuple(
			&TypeFieldReflectionOperator::TypeMeshDataOperator::set_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::get_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getClassName,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldName_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::getFieldTypeName_index_buffer,
			&TypeFieldReflectionOperator::TypeMeshDataOperator::isArray_index_buffer
			);
			REGISTER_TO_FIELD_MAP("index_buffer",f_field_function_tuple_index_buffer);
		
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
			//&TypeFieldReflectionOperator::TypeMeshDataOperator::getMeshDataBaseClassReflectionInstanceList,
            &TypeFieldReflectionOperator::TypeMeshDataOperator::constructorWithJson
            //&TypeFieldReflectionOperator::TypeMeshDataOperator::writeByName
		);
		REGISTER_TO_CLASS_MAP("MeshData",f_class_function_tuple_MeshData);
		}
		namespace TypeWrappersRegister
		{
			void Vertex(){TypeWrapperRegister_Vertex();}
			void MeshData(){TypeWrapperRegister_MeshData();}
		}
		
	}
}

