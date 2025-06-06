#include "global_context.h"
#include "core/log/log_system.h"
#include "function/render/render_system.h"
#include "function/render/window_system.h"
#include <function/framework/world/world_manager.h>
#include <resource/config_manager/config_manager.h>
#include <resource/asset_manager/asset_manager.h>
namespace qyhs
{
	RuntimeGlobalContext g_runtime_global_context;
	bool RuntimeGlobalContext::m_is_game_mode = false;
	void RuntimeGlobalContext::startSystem(const std::string& config_file_path)
	{
		m_config_manager = std::make_shared<ConfigManager>();
		m_config_manager->initialize(config_file_path);

		m_asset_manager = std::make_shared<AssetManager>();
		m_asset_manager->initialize();

		m_logger_system = std::make_shared<LogSystem>();

		m_world_manager = std::make_shared<WorldManager>();
		m_world_manager->initialize();
		
		m_render_system = std::make_shared<RenderSystem>();
		m_render_system->initialize();

		m_input_system = std::make_shared<InputSystem>();
		m_input_system->initialize();
	}
	void RuntimeGlobalContext::createWindow()
	{
		m_window_system = std::make_shared<WindowSystem>();
		WindowCreateInfo info;
		m_window_system->initialize(&info);
	}
}