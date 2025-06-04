#include "tests.h"
#include <iostream>
#include "core/utils/model_importer_gltf.h"
namespace qyhs
{
	void Tests::initialize2()
	{
		QyhsEngine::initialize2();

		//renderer.init(canvas);
		renderer.load();

		activatePath(&renderer);
	}

	void TestRenderer::load()
	{
		gui::Gui& gui = GetGUI();

		test_selector.create("TestSelector");
		test_selector.setText("Demo: ");
		test_selector.setSize(Vector2(140, 20));
		test_selector.setPosition(Vector2(50, 220));
		test_selector.addItem("HelloTriangle",TestType::HELLOTRIANGLE);
		test_selector.addItem("CesiumMan",TestType::CesiumMan);
		test_selector.onSelect([=](gui::EventArgs args){
			switch (args.userdata)
			{
			case HELLOTRIANGLE:
			{
				import_model_gltf(scene, "E://GithubClone//WickedEngine//Content//models//white_triangle.gltf");
			}
			break;
			case CesiumMan:
			{
				import_model_gltf(scene, "E://VS_Project//QyhsEngine//engine//source//runtime//resource//model//CesiumMan//glTF-Embedded//CesiumMan_test.gltf");
			}
			break;
			default:
				break;
			}
			});
		test_selector.setSelected(1);
		gui.addWeights(&test_selector);
		
		RenderPath3D::load();
	}

	void TestRenderer::render()
	{
		RenderPath3D::render();
		
	}

	void TestRenderer::start()
	{
		RenderPath3D::start();
		
	}

}