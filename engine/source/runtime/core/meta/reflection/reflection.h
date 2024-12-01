#pragma once
#include "runtime/core/meta/json.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <tuple>

namespace QYHS
{

#ifdef __REFLECTION_PARSER__
#define META(...)	__attribute__((annotate(#__VA_ARGS__)))
#define CLASS(class_name,...) class __attribute__((annotate(#__VA_ARGS__))) class_name
#define STRUCT(struct_name,...) struct __attribute__((annotate(#__VA_ARGS__))) struct_name
#else
#define META(...)
#define CLASS(class_name,...) class class_name
#define STRUCT(struct_name,...) struct  struct_name
#endif

#define REGISTER_TO_CLASS_MAP(name,value) TypeMetaRegisterInterface::registerToClassMap(name,value);
#define REGISTER_TO_FIELD_MAP(name,value) TypeMetaRegisterInterface::registerToFieldMap(name,value);
#define REGISTER_TO_ARRAY_MAP(name,value) TypeMetaRegisterInterface::registerToArrayMap(name,value);
	// public: virtual std::string getTypeName() override {return #class_name;}

#define REFLECTION_TYPE(class_name) \
    namespace Reflection \
    { \
        namespace TypeFieldReflectionOperator \
        { \
            class Type##class_name##Operator; \
        } \
    };

#define TYPE_META_DEF(class_name,ptr)\
	QYHS::Reflection::ReflectionInstance(QYHS::Reflection::TypeMeta::newMetaFromName(#class_name),(class_name*)ptr)

#define REFLECTION_BODY(class_name)\
	friend class Reflection::TypeFieldReflectionOperator::Type##class_name##Operator;\
	friend class Serializer;
	namespace Reflection
	{
		class TypeMeta;
		class ReflectionInstance;
	}

	typedef std::function<void(void*, void*)>      SetFuncion;
	typedef std::function<void* (void*)>            GetFuncion;
	typedef std::function<const char* ()>           GetNameFuncion;
	typedef std::function<void(int, void*, void*)> SetArrayFunc;
	typedef std::function<void* (int, void*)>       GetArrayFunc;
	typedef std::function<int(void*)>              GetSizeFunc;
	typedef std::function<bool()>                  GetBoolFunc;

	typedef std::function<int(Reflection::ReflectionInstance*&, void*)> GetBaseClassReflectionInstanceListFunc;
	typedef std::function<void* (const Json&)>                          ConstructorWithPJson;
	typedef std::function<Json(void*)>                                 WritePJsonByName;

	typedef std::tuple<SetFuncion, GetFuncion, GetNameFuncion, GetNameFuncion, GetNameFuncion, GetBoolFunc>
		FieldFunctionTuple;
	typedef std::tuple<GetNameFuncion> MethodFunctionTuple;
	typedef std::tuple<ConstructorWithPJson, GetBaseClassReflectionInstanceListFunc,WritePJsonByName>
		ClassFunctionTuple;
	typedef std::tuple<SetArrayFunc, GetArrayFunc, GetSizeFunc, GetNameFuncion, GetNameFuncion> ArrayFunctionTuple;

	namespace Reflection
	{
		static const char* k_unknown_type = "UnKnown Type";
		static const char* k_unknown = "UnKnown";
		class TypeMetaRegisterInterface
		{
		public:
			static void registerToClassMap(const char* name, ClassFunctionTuple* value);
			static void registerToFieldMap(const char* name, FieldFunctionTuple* value);
			static void registerToArrayMap(const char* name, ArrayFunctionTuple* value);
		private:
		};

		class ArrayAccessor
		{
		public:
			ArrayAccessor();
			ArrayAccessor(ArrayFunctionTuple* function_tuple);
			std::string getElementType();
			int getSize(void* instances);
			void* get(int index, void* instance);
		private:
			ArrayFunctionTuple* m_functions;
			int m_array_size;
			void* m_p_array;
		};

		class FieldAccessor
		{
			enum
			{
				_SET_CLASS_FIELD_FUNCTION_ = 0,
				_GET_CLASS_FIELD_FUNCTION_,
				_GET_CLASS_NAME_,
				_GET_FIELD_NAME_,
				_GET_FIELD_TYPE_NAME_,
				_GET_IS_ARRAY_
			};
		public:
			FieldAccessor();
			FieldAccessor(FieldFunctionTuple* function_tuple);
			std::string getFieldTypeName() { return m_field_type_name; }
			bool isArray() { return m_is_array; }
			void* get(void* instance);
			bool getTypeMeta(Reflection::TypeMeta &type_meta);
			std::string getTypeName() { return m_field_type_name; }
		private:
			std::string m_field_type_name;
			std::string m_field_name;
			FieldFunctionTuple *m_functions;
			bool m_is_array{ false };
		};

		class MethodAccessor
		{
		public:
			MethodAccessor();
			MethodAccessor(MethodFunctionTuple* functions);
		private:
			MethodFunctionTuple* m_functions;
			std::string	m_method_name;
		};

		class TypeMeta
		{
		public:
			TypeMeta() {}
			TypeMeta(std::string type_name);
			TypeMeta& operator=(const TypeMeta& type_meta);
			static ReflectionInstance newFromNameAndJson(std::string type_name, const Json& json_context);
			static TypeMeta newMetaFromName(std::string name);
			static bool newArrayAccessorFromName(std::string type_name, ArrayAccessor& array_accessor);
			static Json writeByName(std::string type_name, void* instance_ptr);
			int getBaseClassReflectionInstanceList(Reflection::ReflectionInstance* &reflection_instance_list, void* object_instance);
			int getFields(FieldAccessor*& p_fields);
			std::string getTypeName();
			bool m_is_valid = false;
		private:
			std::string m_type_name;
			std::vector<FieldAccessor, std::allocator<FieldAccessor>> m_fields;
			std::vector<MethodAccessor, std::allocator<MethodAccessor>> m_methods;
		};
		class ReflectionInstance
		{
		public:
			ReflectionInstance();
			ReflectionInstance(TypeMeta meta, void* instance) :m_meta(meta), m_instance(instance) {}

			void* m_instance;
			TypeMeta m_meta;
		private:
		};

		template<typename T>
		class ReflectionPtr
		{
		public:
			ReflectionPtr(std::string type_name, T* instance) :m_type_name(type_name), m_instance(instance) {};
			ReflectionPtr() :m_type_name(), m_instance(nullptr) {};
			void setTypeName(std::string name) { m_type_name = name; }
			T*& getPtrReference() { return m_instance; }
			T* operator->() { return m_instance; }
			T* operator->() const { return m_instance; }
			bool operator==(const T* ptr) { return m_instance == ptr; }
			bool operator!=(const T* ptr) { return m_instance != ptr; }
			std::string getTypeName()const { return m_type_name; }
			
			template<typename T1>
			explicit operator T1* () { return static_cast<T1*>(m_instance); }
			
			T* m_instance;
		private:
			std::string m_type_name;
		};

	};
}