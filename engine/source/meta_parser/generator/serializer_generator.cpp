#include "common/precompile.h"
#include "utils.h"
#include "serializer_generator.h"
#include "template_manager/template_manager.h"

Generator::SerializerGenerator::SerializerGenerator(std::string source_directory, 
													std::function<std::string(std::string)> get_include_function):
										Generator::GeneratorInterface(source_directory + "/_generated/serializer", source_directory, get_include_function)

{
	prepareStatus(m_out_path);
}

int Generator::SerializerGenerator::generate(std::string path, SchemaModule schema)
{
	std::string file_path = processFileName(path);
	Mustache::data mustache_data;
	Mustache::data include_headfiles(Mustache::data::type::list);
	Mustache::data class_defines(Mustache::data::type::list);

	include_headfiles.push_back(Mustache::data("headfile_name", Utils::makeRelativePath(m_root_path, path).string()));
	for (auto class_temp : schema.classes)
	{
		if (!class_temp->shouldCompile())
		{
			continue;
		}
		Mustache::data class_def;
		genClassRenderData(class_temp, class_def);

		for (int index = 0; index < class_temp->m_base_classes.size(); ++index)
		{
			auto include_file = m_get_include_func(class_temp->m_base_classes[index]->name);
			if (!include_file.empty())
			{
				auto include_base_file = processFileName(include_file);
				if (file_path != include_base_file)
				{
					include_headfiles.push_back(Mustache::data("headfile_name", 
										Utils::makeRelativePath(m_root_path, include_base_file).string()));
					
				}
			}
		}
		
		for (auto field : class_temp->m_fields)
		{
			if (!field->shouldCompile())
				continue;
			std::cout << "field name:"<<field->m_name << std::endl;
			// deal vector
			if (field->m_type.find("std::vector") == 0)
			{
				auto include_file = m_get_include_func(field->m_name);
				if (!include_file.empty())
				{
					auto include_file_base = processFileName(include_file);
					if (file_path != include_file_base)
					{
						include_headfiles.push_back(Mustache::data(
							"headfile_name", Utils::makeRelativePath(m_root_path, include_file_base).string()));
					}
				}
			}
			// deal normal
		}
		class_defines.push_back(class_def);
		m_class_defines.push_back(class_def);
	}
	mustache_data.set("class_defines", class_defines);
	mustache_data.set("include_headfiles", include_headfiles);
	std::string render_string = TemplateManager::getInstance()->renderByTemplate("commonSerializerGenFile", mustache_data);

	Utils::saveFile(render_string, file_path);
	m_include_headfiles.push_back(Mustache::data("headfile_name", Utils::makeRelativePath(m_root_path, file_path).string()));
	return 0;
}

void Generator::SerializerGenerator::prepareStatus(std::string& path)
{
	GeneratorInterface::prepareStatus(path);

	//load template
	TemplateManager::getInstance()->loadTemplates(m_root_path, "allSerializer.h");
	TemplateManager::getInstance()->loadTemplates(m_root_path, "allSerializer.ipp");
	TemplateManager::getInstance()->loadTemplates(m_root_path, "commonSerializerGenFile");
}

void Generator::SerializerGenerator::finish()
{
	Mustache::data mustache_data;
	mustache_data.set("class_defines", m_class_defines);
	mustache_data.set("include_headfiles", m_include_headfiles);
	std::string render_string = TemplateManager::getInstance()->renderByTemplate("allSerializer.h", mustache_data);
	Utils::saveFile(render_string, m_out_path + "/all_serializer.h");
	render_string = TemplateManager::getInstance()->renderByTemplate("allSerializer.ipp", mustache_data);
	Utils::saveFile(render_string, m_out_path + "/all_serializer.ipp");
}

std::string Generator::SerializerGenerator::processFileName(std::string path)
{
	auto relative_path = fs::path(path).filename().replace_extension("serializer.gen.h").string();
	return m_out_path + '/' + relative_path;
}

void Generator::GeneratorInterface::prepareStatus(std::string& path)
{
	if (!fs::exists(path))
	{
		fs::create_directories(path);
	}
}


