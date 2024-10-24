#pragma once
#include <function/framework/level/level.h>
#include "resource/type/world_resource.h"
#include <string>
namespace QYHS
{
	class WorldManager
	{
	public:
		void tick(double delta_time);
		void initialize();
		bool loadWorld(const std::string& world_url);
		bool loadLevel(const std::string& level_url);
		Level		m_current_level;
		std::weak_ptr<Level> getCurrentActiveLevel() { return m_current_active_level; }
	private:
		std::shared_ptr<WorldResource> m_current_world_resource;
		bool b_world_load;
		std::string m_current_world_url;
		std::unordered_map<std::string, std::shared_ptr<Level>> m_loaded_levels;
		std::weak_ptr<Level> m_current_active_level;
	};
}