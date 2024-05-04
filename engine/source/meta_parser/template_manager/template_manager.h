#pragma once
#include "common/precompile.h"

class TemplateManager
{
public:
	static TemplateManager* getInstance()
	{
		static TemplateManager* m_instance;
		if (m_instance == nullptr)
		{
			m_instance = new TemplateManager();
		}
		return m_instance;
	}
	std::string renderByTemplate(std::string template_name, Mustache::data& mustache_data);
	void loadTemplates(std::string path, std::string template_name);
private:
	std::unordered_map<std::string, std::string> m_template_pool;
};