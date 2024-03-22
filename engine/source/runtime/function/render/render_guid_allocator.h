#pragma once
#include <unordered_map>
namespace QYHS
{
	static const size_t invalid_value = 0;

	template<typename T>
	class RenderGUIDAllocator
	{
	public:
		template<typename T>
		inline size_t allocateGUId(const T& t)
		{
			auto find_it = m_element_guid_map.find(t);
			if (find_it != m_element_guid_map.end())
			{
				return find_it->second;
			}
			else
			{
				for (size_t i = 0; i < m_guid_element_map.size(); ++i)
				{
					size_t id = i + 1;
					if (m_guid_element_map.find(id) == m_guid_element_map.end())
					{
						m_guid_element_map.insert(std::make_pair(id, t));
						m_element_guid_map.insert(std::make_pair(t, id));
						return id;
					}
				}
				return invalid_value;
			}
		}

		template<typename T>
		bool hasElement(const T& t)
		{
			return m_element_guid_map.find(t) != m_element_guid_map.end();
		}
	private:
		std::unordered_map<size_t, T> m_guid_element_map;
		std::unordered_map<T, size_t>  m_element_guid_map;


	};

}