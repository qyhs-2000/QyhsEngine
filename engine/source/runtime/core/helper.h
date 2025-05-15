#pragma once
#include <filesystem>
namespace qyhs::helper
{
	std::string getCurrentPath();
	bool existFile(const std::string& filename);
	std::string replaceExtension(std::string str, std::string extension);
	std::string getRelativePath(std::string root_dir, std::string path);
	std::string getDirectoryFromPath(std::string filename);
	bool readFile(const std::string& filename, std::vector<uint8_t>& data,size_t offset = 0);
	void stringConvert(const std::string& from, std::wstring& to);
	std::string getFileNameFromPath(std::string path);
	void stringConvert(const std::wstring& from, std::string& to);
	void createDirectory(const std::string& dir_name);
	void makeRelativePath(const std::string& root_dir, std::string& path);
	void makeAbsolutePath(std::string& path);
	std::wstring toNativeString(std::string str);
	bool writeFile(const std::string& filename, const uint8_t* data, size_t data_size);
	std::string getCacheDirectoryPath();
	std::string getExtensionFromFileName(const std::string & filename);
	std::string removeExtension(const std::string& filename);
	uint64_t getNextPowerOfTwo(uint64_t value);
	template<typename T>
	constexpr void hash_combine(std::size_t& seed, const T& v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
}