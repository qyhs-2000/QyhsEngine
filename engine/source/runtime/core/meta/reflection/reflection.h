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
	typedef std::tuple<ConstructorWithPJson>
		ClassFunctionTuple;
	typedef std::tuple<SetArrayFunc, GetArrayFunc, GetSizeFunc, GetNameFuncion, GetNameFuncion> ArrayFunctionTuple;

	namespace Reflection
	{
		class TypeMetaRegisterInterface
		{
		public:
			static void registerToClassMap(const char* name, ClassFunctionTuple* value);
			static void registerToFieldMap(const char* name, FieldFunctionTuple* value);
			static void registerToArrayMap(const char* name, ArrayFunctionTuple* value);
		private:
		};
		class TypeMeta
		{
		public:
			TypeMeta() {}
			TypeMeta(std::string type_name) :m_type_name(type_name) {}
			static ReflectionInstance newFromNameAndJson(std::string type_name, const Json& json_context);
		private:
			std::string m_type_name;
		};
		class ReflectionInstance
		{
		public:
			ReflectionInstance();
			ReflectionInstance(TypeMeta meta, void* instance) :m_meta(meta), m_instance(instance) {}
			void* m_instance;
		private:
			TypeMeta m_meta;
		};


		template<typename T>
		class ReflectionPtr
		{
		public:
			void setTypeName(std::string name) { m_type_name = name; }
			T*& getPtrReference() { return m_instance; }
			T* operator->() { return m_instance; }
			bool operator==(const T* ptr) { return m_instance == ptr; }
			bool operator!=(const T* ptr) { return m_instance != ptr; }
			std::string getTypeName() { return m_type_name; }
		private:
			std::string m_type_name;
			T* m_instance;
		};





	}
}