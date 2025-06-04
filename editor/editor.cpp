#include "editor.h"
#include <iostream>
#include "core/utils/model_importer_gltf.h"
namespace qyhs
{
	void Editor::initialize2()
	{
		QyhsEngine::initialize2();

		//renderer.init(canvas);
		renderer.load();

		activatePath(&renderer);
	}

	void EditorRenderer::load()
	{
		gui::Gui& gui = GetGUI();

		
		RenderPath3D::load();
	}

	void EditorRenderer::render()
	{
		RenderPath3D::render();
		
	}

	void EditorRenderer::start()
	{
		RenderPath3D::start();
		
	}

}