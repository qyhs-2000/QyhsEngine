#pragma once
#include "core/meta/json.h"
#include "core/meta/reflection/reflection.h"
#include <cassert>

namespace QYHS
{
	template<typename...>
	inline constexpr bool always_false = false;
	class Serializer
	{

	public:
		template<typename T>
		static T*& readPointer(const Json& json_context, T*& instance)
		{
			assert(instance == nullptr);
			std::string type_name = json_context["$typeName"].string_value();
			assert(!type_name.empty());
			if ('*' == type_name[0])
			{
				instance = new T;
				read(json_context["$context"], *instance);
			}
			else
			{
				instance = static_cast<T*>(Reflection::TypeMeta::newFromNameAndJson(type_name, json_context["$context"]).m_instance);
			}
			return instance;
		}

		template<typename T>
		static T*& read(const Json& json_context, Reflection::ReflectionPtr<T>& instance)
		{
			assert(instance == nullptr);
			std::string type_name = json_context["$typeName"].string_value();
			instance.setTypeName(type_name);
			return readPointer(json_context, instance.getPtrReference());
		}

		template<typename T>
		static T& read(const Json& json_context, T& instance)
		{
			if constexpr (std::is_pointer<T>::value)
			{
				return readPointer(json_context, instance);
			}
			else
			{
				//static_assert(always_false<T>, "PSerializer::read<T> has not been implemented yet!");
				return instance;
			}
		}
	private:

	};

	// implementation of base types
	
	template<>
	char& Serializer::read(const Json& json_context, char& instance);

	
	template<>
	int& Serializer::read(const Json& json_context, int& instance);

	
	template<>
	unsigned int& Serializer::read(const Json& json_context, unsigned int& instance);

	
	template<>
	float& Serializer::read(const Json& json_context, float& instance);

	
	template<>
	double& Serializer::read(const Json& json_context, double& instance);

	
	template<>
	bool& Serializer::read(const Json& json_context, bool& instance);

	
	template<>
	std::string& Serializer::read(const Json& json_context, std::string& instance);

}