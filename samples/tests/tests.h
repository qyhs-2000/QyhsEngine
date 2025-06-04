#pragma once
#include "engine.h"
#include "function/ui/render_path.h"

namespace qyhs
{
	enum TestType : int
	{
		HELLOTRIANGLE,
		CesiumMan
	};
	
	class TestRenderer :public RenderPath3D
	{
	public:
		virtual void load() override;
		virtual void render() override;
		virtual void start() override;
	private:
		gui::ComboBox test_selector;
		
	};

	class Tests :public QyhsEngine
	{
	public:
		virtual void initialize2() override;
		
	private:
		TestRenderer renderer;
	};
}