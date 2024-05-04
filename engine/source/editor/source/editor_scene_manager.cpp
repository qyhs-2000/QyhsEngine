#include "editor_scene_manager.h"

void QYHS::EditorSceneManager::initialize()
{
	m_camera = std::make_shared<RenderCamera>();
	m_camera->setCameraPosition(glm::vec3(0.f, 0.f, 5.f));

	
}
