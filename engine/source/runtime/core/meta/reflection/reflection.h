#pragma once
#include "runtime/core/meta/json.h"

#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

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

#define REFLECTION_BODY(class_name) \
    friend class Reflection::TypeFieldReflectionOparator::Type##class_name##Operator; \
    friend class PSerializer;
	// public: virtual std::string getTypeName() override {return #class_name;}

#define REFLECTION_TYPE(class_name) \
    namespace Reflection \
    { \
        namespace TypeFieldReflectionOparator \
        { \
            class Type##class_name##Operator; \
        } \
    };


	namespace Reflection
	{
		template<typename T>
		class ReflectionPtr
		{
		public:

		private:

		};

	}
}