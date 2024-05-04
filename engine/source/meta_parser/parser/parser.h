#pragma once
#include "common/precompile.h"
#include "common/namespace.h"
#include "cursor/cursor.h"
#include "generator/generator.h"
#include "common/schema_module.h"
class MetaParser
{
public:
	static void prepare() {};
	MetaParser(std::string project_input_file_name,
		std::string source_include_file_name,
		std::string include_path,
		std::string sys_include,
		std::string module_name,
		bool is_show_error);
	int parse();
	~MetaParser(void);
	void finish(void);
	void generateFiles(void);
	std::string getIncludeFile(std::string name);
private:
	bool parseProject();
	void buildClassAST(const Cursor& cursor, Namespace current_namespace);
private:
	CXIndex           m_index;
	CXTranslationUnit m_translation_unit;
	std::string m_project_input_file;

	std::vector<std::string> m_work_paths;
	std::string              m_module_name;
	std::string              m_sys_include;
	std::string              m_source_include_file_name;
	bool m_is_show_errors;
	std::vector<const char*>                    arguments = { {"-x",
										   "c++",
										   "-std=c++11",
										   "-D__REFLECTION_PARSER__",
										   "-DNDEBUG",
										   "-D__clang__",
										   "-w",
										   "-MG",
										   "-M",
										   "-ferror-limit=0",
										   "-o clangLog.txt"} };
	std::vector<Generator::GeneratorInterface*>   m_generators;
	//type name(like class or field) map to source file name
	std::unordered_map<std::string, std::string> m_type_table;
	/*filename map to schema_module*/
	std::unordered_map<std::string, SchemaModule> m_schema_module;
};