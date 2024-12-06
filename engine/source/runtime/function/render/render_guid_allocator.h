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
			auto find_it = m_element_map_guid.find(t);
			if (find_it != m_element_map_guid.end())
			{
				return find_it->second;
			}
			else
			{
				for (size_t i = 0; i < m_guid_map_element_.size()+1; ++i)
				{
					size_t id = i + 1;
					if (m_guid_map_element_.find(id) == m_guid_map_element_.end())
					{
						m_guid_map_element_.insert(std::make_pair(id, t));
						m_element_map_guid.insert(std::make_pair(t, id));
						return id;
					}
				}
				return invalid_value;
			}
		}

		inline size_t allocateNewGUId()
		{
			T t;
			for (size_t i = 0; i < m_guid_map_element_.size()+1; ++i)
			{
				size_t id = i + 1;
				if (m_guid_map_element_.find(id) == m_guid_map_element_.end())
				{
					m_guid_map_element_.insert(std::make_pair(id, t));
					m_element_map_guid.insert(std::make_pair(t, id));
					return id;
				}
			}
			return invalid_value;
		}

		template<typename T>
		bool hasElement(const T& t)
		{
			return m_element_map_guid.find(t) != m_element_map_guid.end();
		}
	private:
		std::unordered_map<size_t, T> m_guid_map_element_;
		std::unordered_map<T, size_t>  m_element_map_guid;


	};

}