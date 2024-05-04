#include "reflection.h"
#include <cstring>
#include <map>
namespace QYHS
{
	namespace Reflection
	{
		static std::map<std::string, ClassFunctionTuple*>      m_class_map;
		static std::map<std::string, FieldFunctionTuple*>	   m_field_map;
		static std::map<std::string, ArrayFunctionTuple*>	   m_array_map;
		
		ReflectionInstance TypeMeta::newFromNameAndJson(std::string type_name, const Json& json_context)
		{
			auto iter = m_class_map.find(type_name);

			if (iter != m_class_map.end() && iter->second)
			{
				return ReflectionInstance(TypeMeta(type_name), (std::get<0>(*iter->second)(json_context)));
			}
			return ReflectionInstance();
		}

		ReflectionInstance::ReflectionInstance()
		{
		}

		void TypeMetaRegisterInterface::registerToClassMap(const char* name, ClassFunctionTuple* value)
		{
			auto iter = m_class_map.find(name);
			if (iter == m_class_map.end())
			{
				m_class_map.insert_or_assign(name, value);
			}
			else
			{
				delete value;
			}
		}

		void TypeMetaRegisterInterface::registerToFieldMap(const char* name, FieldFunctionTuple* value)
		{
			auto iter = m_field_map.find(name);
			if (iter == m_field_map.end())
			{
				m_field_map.insert_or_assign(name, value);
			}
			else
			{
				delete value;
			}
		}

		void TypeMetaRegisterInterface::registerToArrayMap(const char* name, ArrayFunctionTuple* value)
		{
			auto iter = m_array_map.find(name);
			if (iter == m_array_map.end())
			{
				m_array_map.insert_or_assign(name, value);
			}
			else
			{
				delete value;
			}
		}

	}
}

