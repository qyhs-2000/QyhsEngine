#include "asset_manager.h"
#include "function/global/global_context.h"
#include <resource/config_manager/config_manager.h>
namespace QYHS
{
	void AssetManager::initialize()
	{
	}
	bool AssetManager::loadLevel(const std::string& level_url)
	{
		
		return true;
	}
	std::filesystem::path AssetManager::getFullPath(const std::string& asset_url)
	{

		return std::filesystem::absolute(g_runtime_global_context.m_config_manager->getRootFolder()/ asset_url);
	}
}