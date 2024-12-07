#pragma once
#include <vector>
#include <fstream>
#include <functional>
namespace QYHS
{
	class Utils
	{
	public:
		static std::vector<char> readFile(const std::string& filename) {
			std::ifstream file(filename, std::ios::ate | std::ios::binary);

			if (!file.is_open()) {
				throw std::runtime_error("failed to open file!");
			}

			size_t fileSize = (size_t)file.tellg();
			std::vector<char> buffer(fileSize);

			file.seekg(0);
			file.read(buffer.data(), fileSize);

			file.close();

			return buffer;
		}
	};

	struct FileDialogParam
	{
		enum TYPE
		{
			OPEN,
			SAVE
		} type = OPEN;
		std::string descriptions;
		std::vector<std::string> extensions;
	};

	namespace Helper
	{
		void fileDialog(const FileDialogParam & param,std::function<void(std::string file_name)> on_success_func);
		std::string toUpper(std::string str);
		std::string getFileExtension(std::string file);
		bool fileRead(std::string file,std::vector<uint8_t> & file_data);
		std::string getClassNameExceptNamespace(std::string class_name);
		std::string getNameByFile(std::string file_name);
		std::string getBaseDir(std::string file_path);
	}
}