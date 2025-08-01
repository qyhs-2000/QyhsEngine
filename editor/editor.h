#pragma once
#include "engine.h"
#include "function/ui/render_path.h"
#include "translator.h"
namespace qyhs
{
	class Translator;
	enum TestType : int
	{
		HELLOTRIANGLE,
		CesiumMan
	};
	
	class EditorRenderer :public RenderPath3D
	{
	public:
		virtual void load() override;
		virtual void render() override;
		virtual void start() override;
		void open(const std::string & filename);
		void save(const std::string & filename);
		virtual void compose(CommandList cmd)override;
		virtual void update(float delta_time) override;
		void AddSelected(const scene::PickResult& picked);
		jobsystem::context loadmodel_workload;
		scene::PickResult hovered;
		primitive::Ray pickRay;
		EditorRenderer() {};
		graphics::Texture editorRenderTarget;
		graphics::Texture editorDepthBuffer;
		std::unique_ptr< RenderPath3D> renderPath;
	private:
		//gui::ComboBox test_selector;
		gui::Button open_button;
		gui::Button save_button;
		Translator translator;
	};

	class Editor :public QyhsEngine
	{
	public:
		virtual void initialize2() override;
		Editor() {};
	private:
		EditorRenderer renderer;
	};
}