#include "world_manager.h"
#include "core/base/macro.h"
#include "function/global/global_context.h"
#include <resource/type/world_resource.h>
#include <resource/config_manager/config_manager.h>
#include <resource/asset_manager/asset_manager.h>
#include "core/utils/utils.h"
#include <iostream>
#include <core/utils/model_importer_gltf.h>
namespace qyhs
{
	std::unordered_map<std::string, FileType> WorldManager::file_types =
	{
		{"OBJ",FileType::OBJ},
		{"FBX",FileType::FBX},
		{"GLTF",FileType::GLTF},
		{"GLB",FileType::GLB}
	};

	void WorldManager::tick(double delta_time)
	{
		if (!b_world_load)
		{
			b_world_load = loadWorld(m_current_world_url);

		}
		std::shared_ptr<Level> active_level = m_current_active_level.lock();
		if (active_level)
		{
			active_level->tick(delta_time);
		}
	}

	void WorldManager::initialize()
	{
		b_world_load = false;
		m_current_world_url = g_runtime_global_context.m_config_manager->getDefaultWorldUrl();

	}

	bool WorldManager::loadWorld(const std::string& world_url)
	{
		LOG_INFO("loading world: {}", world_url);
		WorldResource world_res;
		const bool load_world_success = g_runtime_global_context.m_asset_manager->loadAsset(world_url, world_res);
		if (!load_world_success)
		{
			return false;
		}
		m_current_world_resource = std::make_shared<WorldResource>(world_res);
		const bool is_level_load_success = loadLevel(world_res.m_default_level_url);
		if (!is_level_load_success)
		{
			LOG_ERROR("Failed to load level:{}", world_res.m_default_level_url);
			return false;
		}
		auto iter = m_loaded_levels.find(world_res.m_default_level_url);
		if (iter == m_loaded_levels.end())
		{
			LOG_ERROR("Failed to load level:{}", world_res.m_default_level_url);
			return false;
		}
		m_current_active_level = iter->second;
		b_world_load = true;
		return true;
	}

	bool WorldManager::loadLevel(const std::string& level_url)
	{
		std::shared_ptr<Level> level = std::make_shared < Level>();
		const bool level_loaded_successed = level->load(level_url);
		if (!level_loaded_successed)
		{
			return false;
		}
		m_loaded_levels.emplace(level_url, level);
		return true;
	}

	void WorldManager::saveCurrentLevel()
	{
		if (!m_current_active_level.lock())
		{
			LOG_ERROR("no active level,failed to save level");
			return;
		}
		m_current_active_level.lock()->save();
	}

	void WorldManager::loadOBJFile(std::string file_path)
	{
		std::string extension = Helper::toUpper(Helper::getFileExtension(file_path));
		auto iter = file_types.find(extension);
		if (iter == file_types.end())
		{
			std::cout << "Cann't load invalid extension file" << std::endl;
			return;
		}
		if (m_current_active_level.lock())
		{
			std::shared_ptr<Level> current_level = m_current_active_level.lock();
			if (iter->second == FileType::OBJ)
			{
				if (!current_level->importModel_OBJ(file_path))
				{
					throw std::runtime_error("failed to import obj model!");
				}
			}
			else if (iter->second == FileType::GLTF || iter->second == FileType::GLB)
			{
				if (!import_model_gltf(current_level,file_path))
				{
					throw std::runtime_error("failed tot import gltf model!");
				}
			}

		}
	}

}