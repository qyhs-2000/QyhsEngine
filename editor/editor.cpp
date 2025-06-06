#include "editor.h"
#include <iostream>
#include "core/utils/model_importer_gltf.h"
#include "core/utils/utils.h"
#include "core/event/event_handler.h"

enum class FileType
{
	INVALID,
	GLTF
};

static std::unordered_map<std::string, FileType> filetypes = {
	{"GLTF",FileType::GLTF}
};

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
		open_button.create("Open");
		open_button.setText("Open");
		open_button.setPosition(Vector2(1100, 20));
		open_button.setColor(Color(50, 100, 255, 180), gui::WIDGET_STATE::IDLE);
		open_button.setColor(Color(120, 160, 255, 255), gui::WIDGET_STATE::FOCUS);
		open_button.setSize(Vector2(70, 30));
		open_button.onClickFunc([&](gui::EventArgs) {
			std::cout << "Click Open Button" << std::endl;
			helper::FileDialogParam param;
			param.type = helper::FileDialogParam::OPEN;
			param.descriptions = ".gltf";
			param.extensions.push_back("gltf");
			helper::fileDialog(param, [&](std::string filename) {
				eventhandler::subscribe_once(eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
					open(filename);
					});
				});
			});

		save_button.create("Save");
		save_button.setText("Save");
		save_button.setPosition(Vector2(1200, 20));
		save_button.setColor(Color(50, 180, 100, 180), gui::WIDGET_STATE::IDLE);
		save_button.setColor(Color(50, 220, 140, 255), gui::WIDGET_STATE::FOCUS);
		save_button.setSize(Vector2(70, 30));

		gui.addWeights(&open_button);
		gui.addWeights(&save_button);

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

	void EditorRenderer::open(const std::string& filename)
	{
		std::string extension = helper::toUpper(helper::getExtensionFromFileName(filename));
		FileType file_type = FileType::INVALID;
		auto iter = filetypes.find(extension);
		if (iter != filetypes.end())
		{
			file_type = iter->second;
		}
		if (file_type == FileType::INVALID)
		{
			return;
		}
		jobsystem::execute(loadmodel_workload, [=](jobsystem::JobArgs args) {
			std::shared_ptr<scene::Scene> scene = std::make_shared<scene::Scene>();
			if (file_type == FileType::GLTF)
			{
				import_model_gltf(scene.get(), filename);
			}
			eventhandler::subscribe_once(eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
				getCurrentScene().merge(*scene);
				});
			});

	}

}