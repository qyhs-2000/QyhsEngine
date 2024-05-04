#pragma once
#include <memory>
#include "function/render/render_camera.h"
namespace QYHS
{
	class EditorSceneManager
	{
	public:
		std::shared_ptr<RenderCamera> getEditorCamera(){ return m_camera; };
		void initialize();
	private:
		std::shared_ptr<RenderCamera>    m_camera;
	};
}