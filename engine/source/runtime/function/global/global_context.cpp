#include "global_context.h"
#include "core/log/log_system.h"
#include "function/render/render_system.h"
#include "function/render/window_system.h"
#include <function/framework/world/world_manager.h>
#include <resource/config_manager/config_manager.h>
#include <resource/asset_manager/asset_manager.h>
namespace QYHS
{
	RuntimeGlobalContext g_runtime_global_context;
	void RuntimeGlobalContext::startSystem(const std::string& config_file_path)
	{
		m_config_manager = std::make_shared<ConfigManager>();
		m_config_manager->initialize(config_file_path);

		m_asset_manager = std::make_shared<AssetManager>();
		m_asset_manager->initialize();
		m_logger_system = std::make_shared<LogSystem>();
		m_world_manager = std::make_shared<WorldManager>();
		m_world_manager->initialize();
		m_window_system = std::make_shared<WindowSystem>();
		m_render_system = std::make_shared<RenderSystem>();
		WindowCreateInfo info;
		m_window_system->initialize(&info);
		m_render_system->initialize();
	}
}