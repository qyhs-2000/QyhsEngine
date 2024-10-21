#pragma once
#include <memory>
#include "function/render/render_camera.h"
#include "function/render/render_system.h"
namespace QYHS
{
	class EditorSceneManager
	{
	public:
		std::shared_ptr<RenderCamera> getEditorCamera(){ return m_camera; };
		void initialize();
		uint32_t getMeshIDByPickedUV(Vector2 picked_id);
		void GObjectSelected(size_t go_id);
	private:
		std::shared_ptr<RenderCamera>    m_camera;
		std::shared_ptr<RenderSystem>    m_render_system;
	};
}