#include "helper.h"
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
#include <algorithm>
namespace qyhs::helper
{
	std::string getCurrentPath()
	{
		auto path = std::filesystem::current_path();
		return path.u8string();
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

	int stringConvert(const wchar_t* from, char* to, int dest_size_in_characters)
	{
#ifdef _WIN32
		int num = WideCharToMultiByte(CP_UTF8, 0, from, -1, NULL, 0, NULL, NULL);
		if (num > 0)
		{
			if (dest_size_in_characters >= 0)
			{
				num = num<dest_size_in_characters?num:dest_size_in_characters;
			}
			WideCharToMultiByte(CP_UTF8, 0, from, -1, &to[0], num, NULL, NULL);
		}
		return num;
#else
		std::wstring_convert<std::codecvt_utf8<wchar_t>> cv;
		auto result = cv.to_bytes(from).c_str();
		int num = (size_t)cv.converted();
		if (dest_size_in_characters >= 0)
		{
			num = std::min(num, dest_size_in_characters);
		}
		std::memcpy(to, result, num * sizeof(char));
		return num;
#endif // _WIN32
	}

	void stringConvert(const std::wstring & from,std::string & to)
	{
		int size = WideCharToMultiByte(CP_UTF8,0,from.c_str(),-1,NULL,0,NULL,NULL);
		if (size > 0)
		{
			to.resize(size_t(size) - 1);
			WideCharToMultiByte(CP_UTF8, 0, from.c_str(), -1, &to[0], size,NULL,NULL);
		}
	}

	void createDirectory(const std::string& dir_name)
	{
		std::filesystem::create_directories(toNativeString(dir_name));
	}

	void makeRelativePath(const std::string& root_dir, std::string& path)
	{
		if (root_dir.empty() || path.empty())
		{
			return;
		}
		std::filesystem::path filepath = toNativeString(path);
		if (filepath.is_absolute())
		{
			std::filesystem::path root_path = toNativeString(root_dir);
			std::filesystem::path relative_path = std::filesystem::relative(filepath, root_path);
			if (!relative_path.empty())
			{
				path = relative_path.generic_u8string();
			}
		}
	}

	void makeAbsolutePath(std::string& path)
	{
		std::filesystem::path absolute = std::filesystem::absolute(toNativeString(path));
		if (!absolute.empty())
		{
			path = absolute.generic_u8string();
		}
	}

	void splitPath(const std::string& fullpath, std::string& dir, std::string filename)
	{
		size_t iter = fullpath.rfind("/");
		dir = fullpath.substr(0, iter + 1);
		filename = fullpath.substr(iter + 1);
	}

	std::string getFileNameFromPath(std::string path)
	{
		std::string dir, filename;
		splitPath(path, dir, filename);
		return filename;
	}

	std::string getExtensionFromFileName(const std::string& filename)
	{
		auto it = filename.rfind('.');
		if (it == std::string::npos)
		{
			return "";
		}
		return filename.substr(it+1);
	}

	std::string removeExtension(const std::string& filename)
	{
		auto it = filename.rfind('.');
		if (it == std::string::npos)
		{
			return filename;
		}
		return filename.substr(0, it);
	}

	uint64_t fileTimeStamp(std::string name)
	{
		if (!fileExit(name))
		{
			return 0;
		}
		auto time = std::filesystem::last_write_time(toNativeString(name));
		return std::chrono::duration_cast<std::chrono::duration<uint64_t>>(time.time_since_epoch()).count();
	}

	bool fileExit(std::string name)
	{
		return std::filesystem::exists(toNativeString(name));
	}

	std::string toUpper(const std::string& s)
	{
		std::string result;
		std::locale loc;
		for (unsigned int i = 0; i < s.length(); ++i)
		{
			result += std::toupper(s.at(i), loc);
		}
		return result;
	}

	std::wstring toNativeString(std::string str)
	{
		std::wstring file_wide_name;
		stringConvert(str, file_wide_name);
		return file_wide_name;
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

	bool writeFile(const std::string& filename, const uint8_t* data, size_t data_size)
	{
		if (data_size <= 0) 
			return false;
		std::ofstream file(toNativeString(filename), std::ios::binary | std::ios::trunc);
		if (file.is_open())
		{
			file.write((const char*)data,(std::streamsize)data_size);
			return true;
		}
		return false;
	}

	std::string getCacheDirectoryPath()
	{
		auto path = std::filesystem::temp_directory_path();
		return path.generic_u8string();
	}

	uint64_t getNextPowerOfTwo(uint64_t x)
	{
		--x;
		x |= x >> 1;
		x |= x >> 2;
		x |= x >> 4;
		x |= x >> 8;
		x |= x >> 16;
		x |= x >> 32u;
		return ++x;
	}

	bool existFile(const std::string& filename)
	{
		bool exist = std::filesystem::exists(toNativeString(filename));
		return exist;
	}

	std::string replaceExtension(std::string str, std::string extension)
	{
		{
			size_t it = str.rfind('.');
			if (it == std::string::npos)
			{
				return str + "." + extension;
			}
			return str.substr(0, it + 1) + extension;
		}
	}

	std::string getRelativePath(std::string root_dir, std::string path)
	{
		if (root_dir.empty() || path.empty())
		{
			return path;
		}
		std::filesystem::path file_path(qyhs::helper::toNativeString(path));
		if (file_path.is_absolute())
		{
			std::filesystem::path root_path(helper::toNativeString(root_dir));
			std::filesystem::path relative_path = std::filesystem::relative(file_path, root_path);
			return relative_path.generic_u8string();
		}
		return file_path.generic_u8string();
	}

	std::string getDirectoryFromPath(std::string filename)
	{
		if (filename.empty())
		{
			return filename;
		}

		std::string ret, empty;
		splitPath(filename, ret, empty);
		return ret;
	}

	bool readFile(const std::string& filename, std::vector<uint8_t>& data,size_t offset)
	{
		std::ifstream file(toNativeString(filename), std::ios::binary | std::ios::ate);
		if (file.is_open())
		{
			size_t data_size = (size_t)file.tellg() - offset;
			file.seekg((std::streampos)offset);
			data.resize(data_size);
			file.read((char*)data.data(), data_size);
			file.close();
			return true;
		}
		else
		{
			return false;
		}
	}
	
}