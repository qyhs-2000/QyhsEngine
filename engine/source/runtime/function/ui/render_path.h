#pragma once

#include "canvas.h"
#include "gui.h"
#include "function/render/scene.h"
#include "function/render/renderer.h"

namespace qyhs
{
	class RenderPath :public Canvas
	{
	public:
		virtual void deactivate() {};
		virtual void render() {};
		virtual void start() {};
		virtual void compose(CommandList cmd) {};
		virtual void resizeBuffers() {};
		virtual void update(float delta_time) {};
		
	};

	class RenderPath2D :public RenderPath
	{
	public:
		gui::Gui GUI;
		virtual void load() {};
		const gui::Gui& GetGUI()const { return GUI; }
		gui::Gui& GetGUI() { return GUI; }
		virtual void render();
		virtual void start() override {};
		virtual void compose(CommandList cmd) override;
		virtual void resizeBuffers() override;
		float resolution_scale = 1.0;
		XMUINT2 getInternalResolution()const { return XMUINT2((float)getPhysicalWidth() * resolution_scale, (float)getPhysicalHeight()* resolution_scale); }
		virtual void update(float delta_time) override;
		Texture& getRenderResult() { return rt_final; }
		uint32_t getMSAASampleCount()const { return msaa_sample_count; }

		float crop_left = 0.0;
		float crop_right = 0.0;
		float crop_top = 0.0;
		float crop_bottom = 0.0;
		float resolutionScale = 1.0f;

		XMFLOAT4 originalMouse = XMFLOAT4(0, 0, 0, 0);
		XMFLOAT4 currentMouse = XMFLOAT4(0, 0, 0, 0);
	protected:
		Texture rt_final;
		uint32_t msaa_sample_count = 1;
	};

	class RenderPath3D :public RenderPath2D
	{
	public:
		virtual void start()override;
		virtual void render();
		void renderTransparents(CommandList cmd) const;
		void renderOpaques(CommandList cmd) const;
		virtual void compose(CommandList cmd) override;
		virtual void resizeBuffers() override;
		virtual void update(float delta_time) override;
		Texture rt_post_process;
		XMFLOAT3 cam_move = {};
		const graphics::Texture* last_post_process_render_target = &rt_post_process;
		const Texture* getLastPostprocessRT()const {
			return last_post_process_render_target;
		}
		uint32_t layer_mask = 0xFFFFFFFF;
		graphics::Rect getScissorInternalResolution()const;
		scene::CameraComponent* camera = &scene::getCamera();
		scene::TransformComponent camera_transform;
		graphics::Texture rt_main_render;
		graphics::Texture rt_main;
		renderer::Visibility visibility_main;
		graphics::Texture depth_buffer_main;
		scene::Scene* scene = scene::getScene();
		scene::Scene& getCurrentScene() { return *scene; }
		bool occlusion_culling_enabled = true;
		FrameConstantBuffer frame_cb = {};
	public:
		graphics::Texture gui_blurred_background;
	};

}