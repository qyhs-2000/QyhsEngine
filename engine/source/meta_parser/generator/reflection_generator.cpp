#include "reflection_generator.h"
#include "template_manager/template_manager.h"
#include <utils.h>
Generator::ReflectionGenerator::ReflectionGenerator(std::string source_directory, std::function<std::string(std::string)> get_include_function) :
	Generator::GeneratorInterface(source_directory + "/_generated/reflection", source_directory, get_include_function)
{
	prepareStatus(m_out_path);
}

std::string Generator::ReflectionGenerator::processFileName(std::string path)
{
	auto relative_dir = fs::path(path).filename().replace_extension("reflection.gen.h").string();
	return m_out_path + "/" + relative_dir;
}


int Generator::ReflectionGenerator::generate(std::string path, SchemaModule schema)
{
	std::string file_path = processFileName(path);
	Mustache::data mustache_data;
	Mustache::data include_headfiles(Mustache::data::type::list);
	Mustache::data class_defines(Mustache::data::type::list);

	include_headfiles.push_back(
		Mustache::data("headfile_name", Utils::makeRelativePath(m_root_path, path).string()));
	std::set<std::string> class_names;

	for (auto class_temp : schema.classes)
	{
		if (!class_temp->shouldCompile())
			continue;
		class_names.insert(class_temp->getClassName());
		Mustache::data class_define;
		Mustache::data vector_defines(Mustache::data::type::list);

		std::string vector_prefix = "std::vector<";
		genClassRenderData(class_temp, class_define);
		std::map<std::string, std::pair<std::string, std::string>> vector_map;
		for (auto field : class_temp->m_fields)
		{
			if (!field->shouldCompile())
				continue;
			bool is_array = field->m_type.find(vector_prefix) == 0;
			if (is_array)
			{
				std::string array_useful_name = field->m_type;
				Utils::formatQualifiedName(array_useful_name);
				Utils::replaceAll(array_useful_name," ", "");
				std::string item_type = Utils::getNameWithoutContainer(field->m_type);
				vector_map[field->m_type] = std::make_pair(array_useful_name, item_type);
			}
		}
		if (vector_map.size() > 0)
		{
			if (class_define.get("vector_exist") == nullptr)
			{
				class_define.set("vector_exist", true);
			}
			for (auto vector_item : vector_map)
			{
				std::string array_useful_name = vector_item.second.first;
				std::string item_type = vector_item.second.second;
				Mustache::data vector_define;
				vector_define.set("vector_useful_name", array_useful_name);
				vector_define.set("vector_type_name", vector_item.first);
				vector_define.set("vector_element_type_name", item_type);
				vector_defines.push_back(vector_define);
			}
		}
		class_define.set("vector_defines", vector_defines);
		class_defines.push_back(class_define);
		//m_class_defines.push_back(class_def);
	}
	for (auto class_item : class_names)
	{
		m_type_list.emplace_back(class_item);
	}
	mustache_data.set("class_defines", class_defines);
	mustache_data.set("include_headfiles", include_headfiles);
	std::string render_string =
		TemplateManager::getInstance()->renderByTemplate("commonReflectionGenFile", mustache_data);
	Utils::saveFile(render_string, file_path);
	m_head_file_list.emplace_back(Utils::makeRelativePath(m_root_path, file_path).string());
	return 0;
}

void Generator::ReflectionGenerator::prepareStatus(std::string& path)
{
	GeneratorInterface::prepareStatus(path);

	TemplateManager::getInstance()->loadTemplates(m_root_path, "commonReflectionGenFile");
	TemplateManager::getInstance()->loadTemplates(m_root_path, "allReflectionFile");
}

void Generator::ReflectionGenerator::finish()
{
	Mustache::data mustache_data;
	Mustache::data class_defines(Mustache::data::type::list);
	Mustache::data include_headfiles(Mustache::data::type::list);

	for (auto class_item : m_type_list)
	{
		class_defines.push_back(Mustache::data("class_name", class_item));
	}
	for (auto head_file : m_head_file_list)
	{
		include_headfiles.push_back(Mustache::data("headfile_name", head_file));
	}
	mustache_data.set("class_defines", class_defines);
	mustache_data.set("include_headfiles", include_headfiles);
	std::string render_string =
		TemplateManager::getInstance()->renderByTemplate("allReflectionFile", mustache_data);
	Utils::saveFile(render_string, m_out_path + "/all_reflection.h");
}

