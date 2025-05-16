#include "reflection.h"
#include <cstring>
#include <map>
namespace qyhs
{
	namespace Reflection
	{
		static std::map<std::string, ClassFunctionTuple*>      m_class_map;
		static std::map<std::string, FieldFunctionTuple*>	   m_field_map;
		static std::map<std::string, MethodFunctionTuple*>	   m_method_map;
		static std::map<std::string, ArrayFunctionTuple*>	   m_array_map;

		TypeMeta::TypeMeta(std::string type_name) :m_type_name(type_name)
		{
			m_fields.clear();
			m_methods.clear();
			m_is_valid = false;

			auto field_iter = m_field_map.equal_range(type_name);
			while (field_iter.first != field_iter.second)
			{
				FieldAccessor field = FieldAccessor(field_iter.first->second);
				m_fields.emplace_back(field);
				m_is_valid = true;
				++field_iter.first;
			}

			auto method_iter = m_method_map.equal_range(type_name);
			while (method_iter.first != method_iter.second)
			{
				MethodAccessor method = MethodAccessor(method_iter.first->second);
				m_methods.emplace_back(method);
				m_is_valid = true;
				++method_iter.first;
			}
		}

		TypeMeta& TypeMeta::operator=(const TypeMeta& type_meta)
		{
			if (this == &type_meta)
			{
				return *this;
			}
			m_fields.clear();
			m_fields = type_meta.m_fields;


			m_methods.clear();
			m_methods = type_meta.m_methods;

			m_type_name = type_meta.m_type_name;
			m_is_valid  = type_meta.m_is_valid;

			return *this;
		}

		ReflectionInstance TypeMeta::newFromNameAndJson(std::string type_name, const Json& json_context)
		{
			auto iter = m_class_map.find(type_name);

			if (iter != m_class_map.end() && iter->second)
			{
				return ReflectionInstance(TypeMeta(type_name), (std::get<0>(*iter->second)(json_context)));
			}
			return ReflectionInstance();
		}

		TypeMeta TypeMeta::newMetaFromName(std::string name)
		{
			TypeMeta type_meta(name);
			return type_meta;
		}

		bool TypeMeta::newArrayAccessorFromName(std::string type_name, ArrayAccessor& array_accessor)
		{
			auto iter = m_array_map.find(type_name);
			if (iter == m_array_map.end())
			{
				return false;
			}
			ArrayAccessor accessor(iter->second);
			array_accessor = accessor;
			return true;
		}

		Json TypeMeta::writeByName(std::string type_name, void* instance_ptr)
		{
			auto iter = m_class_map.find(type_name);
			if (iter != m_class_map.end())
			{
				return std::get<2>(*iter->second)(instance_ptr);
			}
			return Json();
		}

		int TypeMeta::getBaseClassReflectionInstanceList(Reflection::ReflectionInstance*& reflection_instance_list, void* object_instance)
		{
			auto iter = m_class_map.find(m_type_name);
			if (iter != m_class_map.end())
			{
				return (std::get<1>(*iter->second))(reflection_instance_list, object_instance);
			}
			return 0;
		}

		int TypeMeta::getFields(FieldAccessor* &p_fields)
		{
			int count = m_fields.size();
			p_fields = new FieldAccessor[count];
			for (int i = 0; i < count; ++i)
			{
				p_fields[i] = m_fields[i];
			}
			return count;
		}

		std::string TypeMeta::getTypeName()
		{
			return m_type_name;
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

		FieldAccessor::FieldAccessor()
		{
			m_field_name = k_unknown;
			m_field_type_name = k_unknown_type;
			m_functions = nullptr;
		}

		FieldAccessor::FieldAccessor(FieldFunctionTuple* function_tuple) :m_functions(function_tuple)
		{
			m_field_type_name = k_unknown_type;
			m_field_name = k_unknown;
			if (function_tuple == nullptr)
			{
				return;
			}
			m_field_type_name = std::get<_GET_FIELD_TYPE_NAME_>(*function_tuple)();
			m_field_name = std::get<_GET_FIELD_NAME_>(*function_tuple)();
			m_is_array = std::get<_GET_IS_ARRAY_>(*function_tuple)();

		}

		void* FieldAccessor::get(void* instance)
		{
			return static_cast<void*>((std::get<1>(*m_functions))(instance));
		}

		bool FieldAccessor::getTypeMeta(Reflection::TypeMeta &type_meta)
		{
			TypeMeta field_meta(m_field_type_name);
			type_meta = field_meta;
			return field_meta.m_is_valid;
		}

		MethodAccessor::MethodAccessor()
		{
			m_method_name = k_unknown;
			m_functions = nullptr;
		}

		MethodAccessor::MethodAccessor(MethodFunctionTuple* functions) :m_functions(functions)
		{
			m_method_name = k_unknown;
			m_functions = nullptr;
			if (functions == nullptr)
			{
				return;
			}
			m_method_name = (std::get<0>(*m_functions))();
		}

		ArrayAccessor::ArrayAccessor()
		{
			m_array_size = 0;
			m_p_array = nullptr;
		}

		ArrayAccessor::ArrayAccessor(ArrayFunctionTuple* function_tuple)
		{
			m_array_size = 0;
			m_p_array = nullptr;
			if (function_tuple == nullptr)
			{
				return;
			}
			m_functions = function_tuple;

		}

		std::string ArrayAccessor::getElementType()
		{
			return (std::get<4>(*m_functions))();
		}

		int ArrayAccessor::getSize(void* instances)
		{
			return (std::get<2>(*m_functions))(instances);
		}

		void* ArrayAccessor::get(int index, void* instance)
		{
			return std::get<1>(*m_functions)(index,instance);
		}

	}
}

