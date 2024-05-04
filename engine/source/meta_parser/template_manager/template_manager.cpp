#include "template_manager.h"
#include "utils.h"
std::string TemplateManager::renderByTemplate(std::string template_name, Mustache::data& mustache_data)
{
	std::unordered_map<std::string, std::string>::iterator iter = m_template_pool.find(template_name);
	if (iter == m_template_pool.end())
	{
		return "";
	}
	Mustache::mustache tmpl(m_template_pool[template_name]);
	return tmpl.render(mustache_data);
}

void TemplateManager::loadTemplates(std::string path, std::string template_name)
{
	m_template_pool.insert_or_assign(template_name, Utils::loadFile(path + "/../template/" + template_name + ".mustache"));
}
