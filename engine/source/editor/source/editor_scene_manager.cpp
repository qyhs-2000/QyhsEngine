#include "editor_scene_manager.h"
#include "function/global/global_context.h"

void QYHS::EditorSceneManager::initialize()
{
	m_camera = g_runtime_global_context.m_render_system->getRenderCamera();
	m_render_system = g_runtime_global_context.m_render_system;
}

uint32_t QYHS::EditorSceneManager::getMeshIDByPickedUV(Vector2 picked_id)
{
	return m_render_system->getMeshIDByPickedUV(picked_id);
}

void QYHS::EditorSceneManager::GObjectSelected(size_t go_id)
{
}
