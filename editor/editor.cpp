#include "editor.h"
#include <iostream>
#include "core/utils/model_importer_gltf.h"
#include "core/utils/utils.h"
#include "core/event/event_handler.h"
#include "function/file/archive.h"
#include "function/input/input.h"
enum class FileType
{
	INVALID,
	GLTF,
	XSCENE
};

static std::unordered_map<std::string, FileType> filetypes = {
	{"GLTF",FileType::GLTF},
	{"XSCENE",FileType::XSCENE}
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
		open_button.setPosition(Vector2(800, 20));
		open_button.setColor(Color(50, 100, 255, 180), gui::WIDGET_STATE::IDLE);
		open_button.setColor(Color(120, 160, 255, 255), gui::WIDGET_STATE::FOCUS);
		open_button.setSize(Vector2(70, 30));
		open_button.onClickFunc([&](gui::EventArgs) {
			std::cout << "Click Open Button" << std::endl;
			helper::FileDialogParam param;
			param.type = helper::FileDialogParam::OPEN;
			param.descriptions = ".gltf,.xscene";
			param.extensions.push_back("gltf");
			param.extensions.push_back("xscene");
			helper::fileDialog(param, [&](std::string filename) {
				eventhandler::subscribe_once(eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
					open(filename);
					});
				});
			});

		save_button.create("Save");
		save_button.setText("Save");
		save_button.setPosition(Vector2(900, 20));
		save_button.setColor(Color(50, 180, 100, 180), gui::WIDGET_STATE::IDLE);
		save_button.setColor(Color(50, 220, 140, 255), gui::WIDGET_STATE::FOCUS);
		save_button.setSize(Vector2(70, 30));
		save_button.onClickFunc([&](gui::EventArgs) {
			std::cout << "Click Save Button" << std::endl;
			helper::FileDialogParam param;
			param.type = helper::FileDialogParam::SAVE;
			param.descriptions = "Qyhs Scene(.xscene) | GLTF Model(.gltf)";
			param.extensions.push_back("gltf");
			param.extensions.push_back("xscene");
			helper::fileDialog(param, [=](std::string filename) {
				eventhandler::subscribe_once(eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
					auto extension = helper::toUpper(helper::getExtensionFromFileName(filename));
					std::string file_name = (!extension.compare("GLTF") || !extension.compare("GLB") || !extension.compare("H")) ? filename : helper::forceExtension(filename, param.extensions.front());
					save(filename);
					});
				});

			});

		gui.addWeights(&open_button);
		gui.addWeights(&save_button);

		RenderPath3D::load();
	}

	void EditorRenderer::render()
	{
		RenderPath3D::render();
		RHI* rhi = rhi::getRHI();
		{
			CommandList cmd = rhi->beginCommandList();
			rhi->beginEvent("Editor", cmd);
			Viewport vp;
			vp.width = rhi->getSwapChain()->desc.width;
			vp.height = rhi->getSwapChain()->desc.height;
			rhi->bindViewports(cmd, 1, &vp);
			{
				RenderPassImage rp[] = {
				RenderPassImage::renderTarget(&rt_final,RenderPassImage::LoadOp::LOAD),
				RenderPassImage::depthStencil(&editorDepthBuffer,RenderPassImage::LoadOp::CLEAR,
					RenderPassImage::StoreOp::STORE,
					ResourceState::DEPTHSTENCIL,
					ResourceState::DEPTHSTENCIL,
					ResourceState::DEPTHSTENCIL)
					
				};
				rhi->beginRenderPass(rp,arraysize(rp),cmd);
				Rect scissor = getScissorInternalResolution();
				rhi->bindScissorRects(1, &scissor, cmd);
				translator.Draw(scene::getCamera(), cmd);

				rhi->endRenderPass(cmd);
			}

			rhi->endEvent(cmd);
		}

	}

	void EditorRenderer::start()
	{
		RenderPath3D::start();

		RHI* rhi = rhi::getRHI();
		{
			TextureDesc desc;
			desc.width = rt_final.desc.width;
			desc.height = rt_final.desc.height;
			desc.misc_flags = ResourceMiscFlag::TRANSIENT_ATTACHMENT;
			desc.sample_count = getMSAASampleCount();

			desc.format = Format::D32_FLOAT;
			desc.bind_flags = BindFlag::DEPTH_STENCIL;
			desc.layout = ResourceState::DEPTHSTENCIL;
			rhi->createTexture(&desc, &editorDepthBuffer, nullptr);
			rhi->setName(&editorDepthBuffer, "editor_depthbuffer");

			desc.format = Format::R8G8B8A8_UNORM;
			desc.bind_flags = BindFlag::RENDER_TARGET;
			desc.layout = ResourceState::RENDERTARGET;
			rhi->createTexture(&desc, &editorRenderTarget, nullptr);
			rhi->setName(&editorRenderTarget, "editor_rendertarget");
		}

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
		jobsystem::Execute(loadmodel_workload, [=](jobsystem::JobArgs args) {
			std::shared_ptr<scene::Scene> scene = std::make_shared<scene::Scene>();
			if (file_type == FileType::GLTF)
			{
				import_model_gltf(scene.get(), filename);
			}
			else if (file_type == FileType::XSCENE)
			{
				scene::loadModel(*scene, filename);
			}
			eventhandler::subscribe_once(eventhandler::EVENT_THREAD_SAFE_POINT, [=](uint64_t userdata) {
				getCurrentScene().merge(*scene);
				});
			});

	}

	void EditorRenderer::save(const std::string& filename)
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
		if (file_type == FileType::XSCENE)
		{
			Archive archive = Archive(filename, false);
			if (archive.isOpen())
			{
				scene::Scene& scene = getCurrentScene();
				scene.serialize(archive);
			}
		}
	}

	void EditorRenderer::compose(CommandList cmd)
	{
		RenderPath3D::compose(cmd);
	}

	void EditorRenderer::AddSelected(const scene::PickResult& picked)
	{
		translator.selected.push_back(picked);
	}

	void EditorRenderer::update(float delta_time)
	{
		RenderPath3D::update(delta_time);
		hovered = {};
		pickRay = renderer::GetPickRay((long)currentMouse.x, (long)currentMouse.y, *this, *camera);

		if (hovered.entity == ecs::INVALID_ENTITY)
		{
			if (input::Down(input::MOUSE_BUTTON_LEFT))
			{
				hovered = scene::Pick(pickRay);
				if (hovered.entity == ecs::INVALID_ENTITY && !translator.selected.empty())
				{
					translator.selected.clear();
				}
			}
		}

		if (hovered.entity != ecs::INVALID_ENTITY)
		{
			translator.selected.clear();
			AddSelected(hovered);
		}
		
	}

}