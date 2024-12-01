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

namespace QYHS
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

		void fileDialog(const FileDialogParam & param,std::function<void(std::string file_name)> on_success_func)
		{
			std::thread([=] {
				OPENFILENAME open_file_name;
				wchar_t sz_file[256];
				ZeroMemory(&open_file_name, sizeof(open_file_name));
				open_file_name.lStructSize = sizeof(open_file_name);
				open_file_name.hwndOwner = nullptr;
				open_file_name.lpstrFile = sz_file;
				//it won't work if this line is not added
				open_file_name.lpstrFile[0] = '\0';
				open_file_name.nMaxFile = sizeof(sz_file);
				open_file_name.lpstrFileTitle = NULL;
				open_file_name.nMaxFileTitle = 0;
				open_file_name.lpstrInitialDir = NULL;
				open_file_name.nFilterIndex = 1;

				std::vector<wchar_t> filter;
				filter.reserve(256);
				for (auto& desc : param.descriptions)
				{
					filter.push_back(desc);
				}
				filter.push_back(0);
				for (auto& extension : param.extensions)
				{
					filter.push_back('*');
					filter.push_back('.');
					for (auto& x : extension)
					{
						filter.push_back(x);
					}
					filter.push_back(';');
				}
				filter.push_back(0);
				filter.push_back(0);
				open_file_name.lpstrFilter = filter.data();
				
				bool get_file_name_success = false;
				switch (param.type)
				{
				case FileDialogParam::OPEN:
					open_file_name.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;
					break;
				case FileDialogParam::SAVE:
					open_file_name.Flags = OFN_OVERWRITEPROMPT | OFN_NOCHANGEDIR;
					break;
				default:
					break;
				}
				get_file_name_success = GetOpenFileName(&open_file_name);
				if (get_file_name_success)
				{
					std::string result_file_name;
					stringConvert(open_file_name.lpstrFile, result_file_name);
					on_success_func(result_file_name);
				}
			}).detach();
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
	}
}