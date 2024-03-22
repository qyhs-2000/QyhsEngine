
#include <iostream>
#include "common/precompile.h"
#include "parser/parser.h"

int parse(std::string project_input_file_name,
	std::string source_include_file_name,
	std::string include_path,
	std::string sys_include,
	std::string module_name,
	std::string show_errors)
{
	std::cout << std::endl;
	std::cout << "Parsing meta data for target \"" << module_name << "\"" << std::endl;
	bool is_show_errors = "0" != show_errors;
	MetaParser parser(project_input_file_name, source_include_file_name, include_path, sys_include, module_name, is_show_errors);
	int result = parser.parse();
	if (result)
	{
		return result;
	}
	parser.generateFiles();
	return 0;
}

std::vector<const char*> arguments = {
	"c++",
	"-std=c++17"
	"-D __clang__",
	"-D __META_PARSER__"
};

std::string GetClangString(const CXString& str) {
	std::string c_str = clang_getCString(str);
	clang_disposeString(str);
	return c_str;
}

std::ostream& operator<<(std::ostream& stream, const CXString& str) {
	auto c_str = GetClangString(str);
	stream << c_str;
	return stream;
}

int main(int argc, char* argv[])
{
	auto start_time = std::chrono::system_clock::now();
	int result = 0;
	char* argv_temp[6] = { "E:/VS_Project/QyhsEngine/engine/bin/precompile.json",
						  "E:/VS_Project/QyhsEngine/parser_header.h",
						  "E:/VS_Project/QyhsEngine/engine/source","*","Qyhs","0"};
	if (argv_temp[0] != nullptr && argv_temp[2] != nullptr && argv_temp[3] != nullptr && argv_temp[4] != nullptr && argv_temp[5] != nullptr)
	{
		MetaParser::prepare();
		result = parse(argv_temp[0],argv_temp[1], argv_temp[2], argv_temp[3], argv_temp[4], argv_temp[5]);
		auto duration = std::chrono::system_clock::now() - start_time;
		std::cout << "Completed in " << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() << " ms" << std::endl;
		return result;
	}
	else
	{
		std::cerr << "Arguments parse error!" << std::endl
			<< "Please call the tool like this:" << std::endl
			<< "meta_parser  project_file_name  include_file_name_to_generate  project_base_directory "
			"sys_include_directory module_name showErrors(0 or 1)"
			<< std::endl
			<< std::endl;
		return -1;
	}
	//auto index = clang_createIndex(0, 0);
	//auto translator = clang_parseTranslationUnit(
	//	index, "E:/VS_Project/QyhsEngine/engine/test/source.cpp", arguments.data(), (int)arguments.size(), nullptr, 0, CXTranslationUnit_None);
	//if (!translator) {
	//	throw std::runtime_error("Failed to parse translation unit.");
	//}
	//auto rootCursor = clang_getTranslationUnitCursor(translator);
	//auto childVisitor = [](CXCursor cursor, CXCursor parent, CXClientData data) {
	//	std::cout << "Kind: " << std::setw(20) << clang_getCursorKindSpelling(clang_getCursorKind(cursor)) << "\t"
	//		<< "Cursor: " << std::setw(20) << clang_getCursorSpelling(cursor) << std::endl;
	//	auto cursors = reinterpret_cast<std::vector<CXCursor>*>(data);
	//	if (clang_getCursorKind(cursor) == CXCursor_AnnotateAttr)
	//	{
	//		cursors->emplace_back(parent);
	//	}
	//	
	//	return CXChildVisit_Recurse;
	//	};

	//std::vector<CXCursor> cursors;
	//clang_visitChildren(rootCursor, childVisitor, reinterpret_cast<CXClientData>(&cursors));
	////TODO

	//clang_disposeTranslationUnit(translator);
	//clang_disposeIndex(index);
	
	return 0;
}

