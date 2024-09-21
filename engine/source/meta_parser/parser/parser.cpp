#include "utils.h"
#include "common/precompile.h"
#include "generator/serializer_generator.h"
#include "generator/reflection_generator.h"
#include "common/namespace.h"
#include "language_type/class.h"
#include "parser.h"

#define  TRY_ADD_LANGUAGE_TYPE(handle,container) \
{\
	if(handle->shouldCompile()) \
	{\
		auto file = handle->getSourceFile(); \
		std::cout<<file<<std::endl;\
		m_schema_module[file].container.emplace_back(handle);\
		std::cout<<"m_type_table push_back: "<<file<<std::endl;\
		m_type_table[handle->m_display_name] = file;\
	}else\
	{\
	std::cout<<handle->getClassName()<<" can't compile!"<<std::endl;\
	}\
}

#define RECURSE_NAMESPACES(kind,cursor,method,namespaces) \
{\
	if(kind == CXCursor_Namespace) \
	{\
		auto display_name = cursor.getDisplayName();\
		if(!display_name.empty())\
		{\
			namespaces.emplace_back(display_name);\
			method(cursor,namespaces);\
			namespaces.pop_back();\
		}\
	}\
}

/*
*@param param1 目前project_input_file是一个json文件，里面是项目要用到的所有文件
*@param param2 include_file_path 是一个.h文件，包含源项目的所有头文件，MetaParser解析完project_input_file之后输出到param2中
* @param param3 include_path为项目的所有源文件的包含目录，比如本项目中的engine/source
* @param param4 要额外增加的include_path
* @param param5 模块的名字，可能要解析多个项目，因此一个项目定义为一个模块，目前只有Qyhs
*/
MetaParser::MetaParser(const std::string project_input_file,
					   const std::string include_file_path,
					   const std::string include_path,
					   const std::string sys_include,
					   const std::string module_name,
					   bool  is_show_errors) :
	m_project_input_file(project_input_file),
	m_source_include_file_name(include_file_path), m_index(nullptr), m_translation_unit(nullptr),
	m_sys_include(sys_include), m_module_name(module_name), m_is_show_errors(is_show_errors)
{
	m_work_paths = Utils::split(include_path, ";");
	m_generators.emplace_back(new Generator::SerializerGenerator(
		m_work_paths[0], std::bind(&MetaParser::getIncludeFile, this, std::placeholders::_1)));
	m_generators.emplace_back(new Generator::ReflectionGenerator(
		m_work_paths[0], std::bind(&MetaParser::getIncludeFile, this, std::placeholders::_1)));
}

int MetaParser::parse()
{
	bool parse_include = parseProject();
	if (!parse_include)
	{
		std::cout << "failed to parse all include files in project" << std::endl;
		return -1;
	}

	std::cout << "Parsing the whole project " << std::endl;
	int is_show_error = m_is_show_errors ? 1 : 0;
	m_index = clang_createIndex(true, is_show_error);
	std::string pre_include = "-I";
	std::string sys_include_temp;
	if (!(m_sys_include == "*"))
	{
		sys_include_temp = pre_include + m_sys_include;
		arguments.emplace_back(sys_include_temp.c_str());
	}

	//identity the directory that clang can find include files
	auto paths = m_work_paths;
	for (size_t index = 0; index < paths.size();++index)
	{
		paths[index] = pre_include + paths[index];
		arguments.emplace_back(paths[index].c_str());
	}
	std::filesystem::path input_path(m_source_include_file_name);
	if (!std::filesystem::exists(input_path))
	{
		std::cout << "failed to load" << input_path << std::endl;
		return -2;
	}
	m_translation_unit = clang_createTranslationUnitFromSourceFile(m_index, m_source_include_file_name.c_str(),
		static_cast<int>(arguments.size()), arguments.data(), 0, nullptr);
	auto cursor = clang_getTranslationUnitCursor(m_translation_unit);
	Namespace temp_namespace;
	buildClassAST(cursor, temp_namespace);
	temp_namespace.clear();
	return 0;
}

MetaParser::~MetaParser(void)
{

}

void MetaParser::finish(void)
{
	for (auto generator : m_generators)
	{
		generator->finish();
	}
}

void MetaParser::generateFiles(void)
{
	std::cerr << "Start generate runtime schemas(" << m_schema_module.size() << ")..." << std::endl;
	for (auto& schema : m_schema_module)
	{
		for (auto& generator : m_generators)
		{
			generator->generate(schema.first, schema.second);
		}
	}
	finish();
}

std::string MetaParser::getIncludeFile(std::string name)
{
	auto iter = m_type_table.find(name);
	return iter != m_type_table.end() ? iter->second : std::string();
}

//生成一个parser_header.h文件，包含所有需要映射的类
bool MetaParser::parseProject()
{
	std::cout << "Parsing project file" << m_project_input_file << std::endl;
	std::fstream include_txt_file(m_project_input_file,std::ios::in);
	if (include_txt_file.fail())
	{
		std::cout << "failed to load file" << m_project_input_file << std::endl;
	}
	std::stringstream	buffer;
	buffer << include_txt_file.rdbuf();
	std::string context = buffer.str();
	Utils::replaceAll(context, ",", ";");
	auto include_files = Utils::split(context, ";");

	//upload include file to source file
	std::fstream include_file_stream;
	include_file_stream.open(m_source_include_file_name, std::ios::out);
	if (!include_file_stream.is_open())
	{
		std::cout << "Can't open source file:" << m_source_include_file_name << std::endl;
		return false;
	}
	std::cout << "Generating source include file:" << m_source_include_file_name << std::endl;
	std::string output_filename = Utils::getFileName(m_source_include_file_name);
	if (output_filename.empty())
	{
		output_filename = "META_INPUT_HEADER_H";
	}
	else
	{
		Utils::replace(output_filename, ".", "_");
		Utils::replace(output_filename, " ", "_");
		Utils::toUpper(output_filename);
	}
	include_file_stream << "#ifndef" << "__" << output_filename << "__" << std::endl;
	include_file_stream << "#ifdef" << "__" << output_filename << "__" << std::endl;
	for (auto include_file : include_files)
	{
		std::string temp_string(include_file);
		Utils::replace(temp_string, '\\', '/');
		include_file_stream << "#include \"" << include_file << "\"" << std::endl;
	}
	include_file_stream << "#endif" << std::endl;
	include_file_stream.close();
	return true;
}

void MetaParser::buildClassAST(const Cursor& cursor, Namespace current_namespace)
{
	std::string spelling_name = cursor.getSpelling();
	std::string display_name = cursor.getDisplayName();
	std::cout << cursor.getSpelling() << std::endl;
	for (auto& child : cursor.getChildren())
	{
		auto kind = child.getKind();
		if (child.isDefinition() && (kind == CXCursor_ClassDecl || kind == CXCursor_FieldDecl))
		{
			auto class_ptr = std::make_shared<Class>(child, current_namespace);
			TRY_ADD_LANGUAGE_TYPE(class_ptr, classes);
		}
		else
		{
			RECURSE_NAMESPACES(kind, child, buildClassAST, current_namespace);
		}
	}
}
