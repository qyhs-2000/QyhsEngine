#include "utils.h"
#include <thread>
#include <locale>
#include <chrono>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <codecvt> // string conversion
#include <filesystem>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <wchar.h>
#include <Windows.h>
#if defined(_WIN32)
#include <direct.h>
#include <Psapi.h> // GetProcessMemoryInfo
#include <Commdlg.h> // openfile
#include <WinBase.h>
#endif // 
#include <functional>

namespace qyhs
{
	namespace Helper
	{
		void stringConvert(const std::wstring & from,std::string & to)
		{
			int size = WideCharToMultiByte(CP_UTF8,0,from.c_str(),-1,NULL,0,NULL,NULL);
			if (size > 0)
			{
				to.resize(size_t(size) - 1);
				WideCharToMultiByte(CP_UTF8, 0, from.c_str(), -1, &to[0], size, NULL, NULL);
			}
		}

		void stringConvert(const std::string & from,std::wstring & to)
		{
			int size = MultiByteToWideChar(CP_UTF8,0,from.c_str(),-1,NULL,0);
			if (size > 0)
			{
				to.resize(size_t(size) - 1);
				MultiByteToWideChar(CP_UTF8, 0, from.c_str(), -1, &to[0], size);
			}
		}

		std::string toUpper(std::string str)
		{
			std::string result;
			result.reserve(str.size());
			std::locale loc;
			for (auto& c : str)
			{
				result += std::toupper(c, loc);
			}
			return result;
		}
		std::string getFileExtension(std::string file)
		{
			auto index = file.rfind('.');
			if (index != std::string::npos)
			{
				std::string extension = file.substr(index + 1);
				return extension;
			}
			return "";
		}

		std::wstring toNativeString(std::string str)
		{
			std::wstring file_wide_name;
			stringConvert(str, file_wide_name);
			return file_wide_name;
		}

		bool fileRead(std::string file,std::vector<uint8_t> & file_data)
		{
			//WIN API process file path with wide char(UTF-16)
			std::ifstream file_stream(toNativeString(file), std::ios::binary | std::ios::ate);
			if (file_stream.is_open())
			{
				size_t data_size = file_stream.tellg();
				file_stream.seekg(0);
				file_data.resize(data_size);
				file_stream.read((char*)file_data.data(), data_size);
				file_stream.close();
				return true;
			}
			return false;
		}
		std::string getClassNameExceptNamespace(std::string class_name)
		{
			size_t index = class_name.rfind(':');
			if (index == std::string::npos)
			{
				return "";
			}
			return class_name.substr(index+1);
		}
		std::string getNameByFile(std::string file_name)
		{
			std::string file = file_name.substr(file_name.rfind('\\') + 1);
			std::string result = file.substr(0,file.rfind('.'));
			return result;
		}
		std::string getBaseDir(std::string file_path)
		{
			auto iter = file_path.find_last_of("/\\");
			if (iter != std::string::npos)
			{
				return file_path.substr(0, iter);
			}
			return "";
		}
	}
}