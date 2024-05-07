#include "editor_scene_manager.h"
#include "function/global/global_context.h"
#include "function/render/render_system.h"
void QYHS::EditorSceneManager::initialize()
{
	m_camera = g_runtime_global_context.m_render_system->getRenderCamera();
}
