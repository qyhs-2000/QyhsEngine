#pragma once

#include <filesystem>
#include <fstream>
#include <functional>
#include <sstream>
#include <string>
#include "core/base/macro.h"
#include "core/meta/json.h"
#include "core/meta/serializer/serializer.h"
#include <function/framework/level/level.h>

#include "_generated/serializer/all_serializer.h"
namespace QYHS
{
	class AssetManager
	{
	public:
		void initialize();
		template<typename AssetType>
		bool loadAsset(const std::string& asset_url, AssetType& asset)
		{
			std::filesystem::path asset_path = getFullPath(asset_url);
			std::ifstream asset_json_file(asset_path);
			//load json file
			if (!asset_json_file)
			{
				LOG_ERROR("open file:{}", asset_path.generic_string());
				return false;
			}
			//insert content of storage buffer of asset_json_file into string_buffer
			std::stringstream string_buffer;
			string_buffer << asset_json_file.rdbuf();
			std::string asset_json_text(string_buffer.str());

			std::string error;
			auto&& asset_json = Json::parse(asset_json_text, error);
			if (!error.empty())
			{
				LOG_ERROR("parse json file {} failed", asset_url);
				return false;
			}
			Serializer::read(asset_json, asset);
			return true;
		}

		bool loadLevel(const std::string& level_url);
		std::filesystem::path getFullPath(const std::string &asset_url);
		template<typename AssetType>
		bool saveAsset(AssetType& output_asset, std::string asset_url)
		{
			std::ofstream asset_json_file(getFullPath(asset_url));
			if (!asset_json_file)
			{
				LOG_ERROR("Fail to open file {}", asset_url);
				return false;
			}
			// write to json object and dump to string
			Json &&        asset_json      = Serializer::write(output_asset);
			std::string&& asset_json_text = asset_json.dump();

			// write to file
			asset_json_file << asset_json_text;
			asset_json_file.flush();

			return true;
		}
	private:
		std::unordered_map<std::string, std::shared_ptr<Level>>  m_loaded_levels;
	};
}