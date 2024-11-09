#include "generator/generator.h"


void Generator::GeneratorInterface::genClassRenderData(std::shared_ptr<Class> class_temp, Mustache::data& class_def)
{
	std::cout << "Generating Class Render File" << std::endl;
	std::cout << "class_name:" << class_temp->getClassName() << " ";
	class_def.set("class_name", class_temp->getClassName());
	class_def.set("class_base_class_size", std::to_string(class_temp->m_base_classes.size()));
	class_def.set("class_need_register", true);
	std::string class_name = class_temp->getClassName();
	if (class_name == "MeshComponent")
	{
		int a;
	}
	if (class_temp->m_base_classes.size() > 0)
	{
		Mustache::data class_base_class_defines(Mustache::data::type::list);
		class_def.set("class_has_base", true);
		for (int index = 0; index < class_temp->m_base_classes.size(); ++index)
		{
			Mustache::data class_base_class_def;
			class_base_class_def.set("class_base_class_name", class_temp->m_base_classes[index]->name);
			class_base_class_def.set("class_base_class_index", std::to_string(index));
			class_base_class_defines.push_back(class_base_class_def);
		}
		class_def.set("class_base_class_defines", class_base_class_defines);
	}
	Mustache::data class_field_defines = Mustache::data::type::list;
	genFieldRenderData(class_temp, class_field_defines);
	class_def.set("class_field_defines", class_field_defines);
}

void Generator::GeneratorInterface::genFieldRenderData(std::shared_ptr<Class> class_temp, Mustache::data& field_defines)
{
	static const std::string vector_prefix = "std::vector<";
	for (auto& field : class_temp->m_fields)
	{
		if (!field->shouldCompile())
		{
			continue;
		}
		Mustache::data field_define;
		std::cout << "Generating Field :" << std::endl;
		std::cout << "field_name: " << field->m_name << "  ";
		field_define.set("class_field_name", field->m_name);
		field_define.set("class_field_type", field->m_type);
		field_define.set("class_field_display_name", field->m_display_name);
		bool is_vector = field->m_type.find(vector_prefix) == 0;
		field_define.set("class_field_is_vector", is_vector);
		field_defines.push_back(field_define);
	}
}