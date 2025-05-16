#pragma once
#include <memory>
#include <string>
#include <function/input/input_system.h>

namespace qyhs
{
	class AssetManager;
	class ConfigManager;
	class LogSystem;
	class WindowSystem;
	class RenderSystem;
	class WorldManager;


	class RuntimeGlobalContext
	{
		
	public:

		std::shared_ptr<AssetManager>			m_asset_manager;
		std::shared_ptr<ConfigManager>			m_config_manager;
		std::shared_ptr<LogSystem>				m_logger_system;
		std::shared_ptr<WindowSystem>			m_window_system;
		std::shared_ptr<RenderSystem>			m_render_system;
		std::shared_ptr<WorldManager>			m_world_manager;
		std::shared_ptr<InputSystem>			m_input_system;
		static bool m_is_game_mode;
	public:
		void startSystem(const std::string& config_file_path);
		void createWindow();
	private:
		
	};

	extern RuntimeGlobalContext g_runtime_global_context;
}
