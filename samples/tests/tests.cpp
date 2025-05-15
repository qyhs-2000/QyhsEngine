#include "tests.h"
#include <iostream>
namespace qyhs
{
	void Tests::initialize2()
	{
		QyhsEngine::initialize2();

		//renderer.init(canvas);
		renderer.load();

		activatePath(&renderer);
	}

	void TestsRenderer::load()
	{
		gui::Gui& gui = GetGUI();

		test_selector.create("TestSelector");
		test_selector.setText("Demo: ");
		test_selector.setSize(Vector2(140, 20));
		test_selector.setPosition(Vector2(50, 220));
		test_selector.addItem("HelloTriangle",TestType::HELLOTRIANGLE);
		test_selector.onSelect([=](){

			});
		test_selector.setSelected(0);
		gui.addWeights(&test_selector);
		
		RenderPath3D::load();
	}

	void TestsRenderer::render()
	{
		RenderPath3D::render();
		
	}

}