#include "render_path.h"
#include "core/jobsystem.h"
#include "function/render/renderer.h"
#include "core/utils/model_importer_gltf.h"
#include "function/input/input.h"
#include <iostream>

enum class EditorActions
{
	// Camera movement
	MOVE_CAMERA_FORWARD,
	MOVE_CAMERA_BACKWARD,
	MOVE_CAMERA_LEFT,
	MOVE_CAMERA_RIGHT,
	MOVE_CAMERA_UP,
	MOVE_CAMERA_DOWN,

	COUNT
};

struct HotkeyInfo
{
	qyhs::input::BUTTON button;
	bool press = false;
	bool control = false;
	bool shift = false;
};
HotkeyInfo hotkeyActions[size_t(EditorActions::COUNT)] = {
	{qyhs::input::BUTTON('W'),					/*press=*/ false,		/*control=*/ false,		/*shift=*/ false},	//MOVE_CAMERA_FORWARD,
	{qyhs::input::BUTTON('S'),					/*press=*/ false,		/*control=*/ false,		/*shift=*/ false},	//MOVE_CAMERA_BACKWARD,
	{qyhs::input::BUTTON('A'),					/*press=*/ false,		/*control=*/ false,		/*shift=*/ false},	//MOVE_CAMERA_LEFT,
	{qyhs::input::BUTTON('D'),					/*press=*/ false,		/*control=*/ false,		/*shift=*/ false},	//MOVE_CAMERA_RIGHT,
	{qyhs::input::BUTTON('E'),					/*press=*/ false,		/*control=*/ false,		/*shift=*/ false},	//MOVE_CAMERA_UP,
	{qyhs::input::BUTTON('Q'),					/*press=*/ false,		/*control=*/ false,		/*shift=*/ false},	//MOVE_CAMERA_DOWN,

};
static_assert(arraysize(hotkeyActions) == size_t(EditorActions::COUNT));

bool checkInput(EditorActions action)
{
	const HotkeyInfo& hotkey = hotkeyActions[size_t(action)];
	bool ret = false;
	if (hotkey.press)
	{
		ret |= qyhs::input::Press(hotkey.button);
	}
	else
	{
		ret |= qyhs::input::Down(hotkey.button);
	}
	if (hotkey.control)
	{
		ret &= qyhs::input::Down(qyhs::input::KEYBOARD_BUTTON_LCONTROL) || qyhs::input::Down(qyhs::input::KEYBOARD_BUTTON_RCONTROL);
	}
	if (hotkey.shift)
	{
		ret &= qyhs::input::Down(qyhs::input::KEYBOARD_BUTTON_LSHIFT) || qyhs::input::Down(qyhs::input::KEYBOARD_BUTTON_RSHIFT);
	}
	return ret;
}

namespace qyhs
{
	bool camera_control_start = true;
	void RenderPath3D::start()
	{
		resizeBuffers();
		//import_model_gltf(scene, "E://VS_Project//QyhsEngine//engine//source//runtime//resource//model//CesiumMan//glTF-Embedded//CesiumMan_test.gltf");
		//import_model_gltf(scene, "E://GithubClone//WickedEngine//Content//models//feibi_y_up.gltf");
	
		//initialize camera position
		XMMATRIX mat = XMMatrixTranslation(0.f, 2.f, -4.f);
		camera->transformCamera(mat);
		camera->SetDirty();
		camera_transform.MatrixTransform(mat);
		camera_transform.updateTransform();
		camera->updateCamera();
	}

	void RenderPath3D::resizeBuffers()
	{
		RHI* rhi = rhi::getRHI();
		XMUINT2 internalResolution = getInternalResolution();
		camera->width = (float)internalResolution.x;
		camera->height = (float)internalResolution.y;
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = Format::R10G10B10A2_UNORM;
			desc.width = internalResolution.x / 16;
			desc.height = internalResolution.y / 16;
			desc.bind_flags = BindFlag::UNORDERED_ACCESS | BindFlag::SHADER_RESOURCE;
			rhi->createTexture(&desc, &gui_blurred_background);
			//rhi->setName(&rtGUIBlurredBackground[0], "rtGUIBlurredBackground[0]");
			image::setBackground(gui_blurred_background);
		}

		//rt_main
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE | BindFlag::UNORDERED_ACCESS;
			desc.format = renderer::format_rendertarget_main;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			rhi->createTexture(&desc, &rt_main);
			rhi->setName(&rt_main, "rt_main");

			if (getMSAASampleCount() > 1)
			{
				desc.sample_count = getMSAASampleCount();
				desc.bind_flags = BindFlag::RENDER_TARGET | BindFlag::SHADER_RESOURCE;

				rhi->createTexture(&desc, &rt_main_render);
				rhi->setName(&rt_main_render, "rt_main_render");
			}
			else
			{
				rt_main_render = rt_main;
			}
		}
		//depth buffer
		{
			TextureDesc desc;
			desc.width = internalResolution.x;
			desc.height = internalResolution.y;
			desc.sample_count = getMSAASampleCount();
			desc.layout = ResourceState::DEPTHSTENCIL;
			desc.format = renderer::format_depthbuffer_main;
			desc.bind_flags = BindFlag::DEPTH_STENCIL;
			rhi->createTexture(&desc, &depth_buffer_main);
			rhi->setName(&depth_buffer_main, "depth_buffer_main");
		}

		RenderPath2D::resizeBuffers();
	}

	void RenderPath3D::update(float delta_time)
	{
		RHI* rhi = rhi::getRHI();
		RenderPath2D::update(delta_time);

		scene->update(delta_time);

		//Frustum culling for main camera
		visibility_main.layer_mask = layer_mask;
		visibility_main.scene = scene;
		visibility_main.camera = camera;
		visibility_main.flags = renderer::Visibility::ALLOW_EVERYTHING;
		if (!occlusion_culling_enabled)
		{
			visibility_main.flags &= ~renderer::Visibility::ALLOW_OCCLUSION_CULLING;
		}
		renderer::updateVisibility(visibility_main);
		renderer::updatePerFrameData(*scene, visibility_main, frame_cb);

		float x_dif = 0.0f, y_dif = 0.0f;
		currentMouse = input::GetPointer();
		if (camera_control_start)
		{
			originalMouse = currentMouse;
		}
		if (input::Down(input::MOUSE_BUTTON_RIGHT))
		{
			camera_control_start = false;
			x_dif = input::GetMouseState().delta_position.x;
			y_dif = input::GetMouseState().delta_position.y;
			x_dif = 0.1f * x_dif * (1.0f / 60.0f);
			y_dif = 0.1f * y_dif * (1.0f / 60.0f);
			input::SetPointer(originalMouse);
			input::HidePointer(true);
		}
		else
		{
			camera_control_start = true;
			input::HidePointer(false);
		}
		
		const float clamped_delta_time = std::min(0.1f, delta_time);
		const float speed = (input::Down(input::KEYBOARD_BUTTON_LSHIFT) ? 10.0f : 1.0f) * 6 * clamped_delta_time;
		XMVECTOR move = XMLoadFloat3(&cam_move);
		XMVECTOR moveNew = XMVectorSet(0, 0, 0, 0);

		if (!input::Down(input::KEYBOARD_BUTTON_LCONTROL))
		{
			// Only move camera if control not pressed
			if (checkInput(EditorActions::MOVE_CAMERA_LEFT)) { moveNew += XMVectorSet(-1, 0, 0, 0); }
			if (checkInput(EditorActions::MOVE_CAMERA_RIGHT)) { moveNew += XMVectorSet(1, 0, 0, 0); }
			if (checkInput(EditorActions::MOVE_CAMERA_FORWARD)) { moveNew += XMVectorSet(0, 0, 1, 0);  }
			if (checkInput(EditorActions::MOVE_CAMERA_BACKWARD)) { moveNew += XMVectorSet(0, 0, -1, 0); }
			if (checkInput(EditorActions::MOVE_CAMERA_UP)) { moveNew += XMVectorSet(0, 1, 0, 0); }
			if (checkInput(EditorActions::MOVE_CAMERA_DOWN)) { moveNew += XMVectorSet(0, -1, 0, 0); }
			moveNew = XMVector3Normalize(moveNew);
		}

		const XMFLOAT4 leftStick = input::GetAnalog(input::GAMEPAD_ANALOG_THUMBSTICK_L, 0);
		const XMFLOAT4 rightStick = input::GetAnalog(input::GAMEPAD_ANALOG_THUMBSTICK_R, 0);

		moveNew += XMVectorSet(leftStick.x, 0, leftStick.y, 0);
		moveNew *= speed;

		move = XMVectorLerp(move, moveNew, 0.28 * clamped_delta_time / 0.0066f); // smooth the movement a bit
		float move_length = XMVectorGetX(XMVector3Length(move));
		if (move_length < 0.0001f)
		{
			move = XMVectorSet(0, 0, 0, 0);
		}
		if (std::abs(x_dif) + std::abs(y_dif) > 0 || move_length > 0.0001)
		{
			XMMATRIX camera_rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&camera_transform.local_rotation));
			XMVECTOR move_rot = XMVector3TransformNormal(move, camera_rotation);
			XMFLOAT3 _move;
			XMStoreFloat3(&_move, move_rot);
			camera_transform.translate(_move);
			camera_transform.rotateRollPitchYaw(XMFLOAT3(y_dif, x_dif, 0));
			camera_transform.updateTransform();
			camera->SetDirty();
		}
		camera->transformCamera(camera_transform);
		camera->updateCamera();
	}

	void RenderPath2D::render()
	{
		RHI* rhi = rhi::getRHI();
		CommandList command_list = rhi->beginCommandList();
		RenderPassImage rp[] = {
			RenderPassImage::renderTarget(
				&rt_final,
				RenderPassImage::LoadOp::LOAD
			),
		};
		image::setCanvas(*this);
		font::setCanvas(*this);
		rhi->beginRenderPass(rp, arraysize(rp), command_list);
		Viewport vp;
		vp.width = (float)rt_final.desc.width;
		vp.height = (float)rt_final.desc.height;
		rhi->bindViewports(command_list, 1, &vp);
		GetGUI().render(*this, command_list);
		rhi->endRenderPass(command_list);
	}

	void RenderPath3D::render()
	{
		std::shared_ptr<jobsystem::Context> ctx = std::make_shared<jobsystem::Context>();
		RHI* rhi = rhi::getRHI();
		CommandList cmd = rhi->beginCommandList();

		//prepare the frame
		jobsystem::execute(*ctx, [&](jobsystem::JobArgs args) {
			renderer::updateRenderData(visibility_main, frame_cb, cmd);
			});

		jobsystem::wait(*ctx);
		static const uint32_t drawscene_flags = renderer::DRAWSCENE_OPAQUE | renderer::DRAWSCENE_MAINCAMERA;

		//main camera depth prepass
		cmd = rhi->beginCommandList();
		CommandList cmd_maincamera_prepass = cmd;
		jobsystem::execute(*ctx, [this,cmd](jobsystem::JobArgs args) {
			RHI* rhi = rhi::getRHI();
			renderer::bindCameraConstantBuffer(*camera, cmd);
			RenderPassImage rp[] = {
				RenderPassImage::depthStencil(&depth_buffer_main,RenderPassImage::LoadOp::CLEAR,
					RenderPassImage::StoreOp::STORE,
					ResourceState::DEPTHSTENCIL,
					ResourceState::DEPTHSTENCIL,
					ResourceState::DEPTHSTENCIL)
			};
			rhi->beginRenderPass(rp, arraysize(rp), cmd);
			rhi->beginEvent("Opaque Z-prepass", cmd);
			renderer::bindCameraConstantBuffer(*camera, cmd);
			renderer::bindCommonResources(cmd);
			Rect scissor = getScissorInternalResolution();
			rhi->bindScissorRects(1, &scissor, cmd);
			Viewport vp;
			vp.width = (float)depth_buffer_main.desc.width;
			vp.height = (float)depth_buffer_main.desc.height;
			vp.min_depth = 0;
			vp.max_depth = 1;
			rhi->bindViewports(cmd, 1, &vp);
			renderer::drawScene(visibility_main, drawscene_flags, RENDERPASS_PREPASS, cmd);
			rhi->endRenderPass(cmd);
			rhi->endEvent(cmd);
		});

		jobsystem::wait(*ctx);

		//main camera opaque color pass
		cmd = rhi->beginCommandList();
		//rhi->waitCommandList(cmd, cmd_maincamera_prepass);
		jobsystem::execute(*ctx, [this, cmd](jobsystem::JobArgs args) {
			renderer::bindCameraConstantBuffer(*camera, cmd);
			renderer::bindCommonResources(cmd);
			renderOpaques(cmd);
			});
		jobsystem::wait(*ctx);
		//transparents
		jobsystem::execute(*ctx, [this, cmd](jobsystem::JobArgs args) {
			renderer::bindCameraConstantBuffer(*camera, cmd);
			renderer::bindCommonResources(cmd);
			renderTransparents(cmd);
			});

		jobsystem::wait(*ctx);
		RenderPath2D::render();
	}

	graphics::Rect RenderPath3D::getScissorInternalResolution()const
	{
		graphics::Rect scissor;
		scissor.left = int(logicalToPhysical(crop_left) * resolution_scale);
		scissor.top = int(logicalToPhysical(crop_top) * resolution_scale);
		scissor.right = int(getInternalResolution().x - logicalToPhysical(crop_right) * resolutionScale);
		scissor.bottom = int(getInternalResolution().y - logicalToPhysical(crop_bottom) * resolutionScale);
		return scissor;
	}

	void RenderPath3D::renderTransparents(CommandList cmd) const
	{
		RHI* rhi = rhi::getRHI();
		RenderPassImage rp[] = {
			RenderPassImage::renderTarget(&rt_main_render,RenderPassImage::LoadOp::LOAD),
			RenderPassImage::depthStencil(&depth_buffer_main,RenderPassImage::LoadOp::LOAD,
				RenderPassImage::StoreOp::STORE,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL),
				RenderPassImage::resolve(&rt_main),
		};
		rhi->beginRenderPass(rp, getMSAASampleCount() > 1 ? 3 : 2, cmd);
		Rect scissor = getScissorInternalResolution();
		rhi->bindScissorRects(1, &scissor, cmd);

		Viewport vp;
		vp.width = (float)depth_buffer_main.desc.width;
		vp.height = (float)depth_buffer_main.desc.height;
		vp.min_depth = 0;
		vp.max_depth = 1;
		//Transparent scene
		{
			rhi->beginEvent("Transparent Scene", cmd);
			rhi->bindViewports(cmd, 1, &vp);

			renderer::drawScene(visibility_main,renderer::DRAWSCENE_TRANSPARENT|renderer::DRAWSCENE_MAINCAMERA,RENDERPASS_MAIN,cmd);
		}
		renderer::drawDebugWorld(*camera,cmd);
		rhi->endRenderPass(cmd);
	}

	void RenderPath3D::renderOpaques(CommandList cmd) const
	{
		RHI* rhi = rhi::getRHI();
		RenderPassImage rp[] = {
			RenderPassImage::renderTarget(&rt_main_render,RenderPassImage::LoadOp::CLEAR),
			RenderPassImage::depthStencil(&depth_buffer_main,RenderPassImage::LoadOp::LOAD,
				RenderPassImage::StoreOp::STORE,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL,
				ResourceState::DEPTHSTENCIL),
				RenderPassImage::resolve(&rt_main),
		};
		rhi->beginRenderPass(rp, getMSAASampleCount() > 1 ? 3 : 2, cmd);
		Rect scissor = getScissorInternalResolution();
		rhi->bindScissorRects(1, &scissor, cmd);

		Viewport vp;
		vp.width = (float)depth_buffer_main.desc.width;
		vp.height = (float)depth_buffer_main.desc.height;
		vp.min_depth = 0;
		vp.max_depth = 1;
		//Opaque scene
		{
			rhi->beginEvent("Opaque Scene", cmd);
			rhi->bindViewports(cmd, 1, &vp);

			renderer::drawScene(visibility_main,renderer::DRAWSCENE_OPAQUE|renderer::DRAWSCENE_MAINCAMERA,RENDERPASS_MAIN,cmd);
		}
		
		rhi->endRenderPass(cmd);
	}

	void RenderPath3D::compose(CommandList cmd)
	{
		RHI* rhi = rhi::getRHI();
		image::Params param;
		param.blendFlag = BLENDMODE_OPAQUE;
		param.quality = image::QUALITY_LINEAR;
		param.enableFullScreen();

		rhi->beginEvent("Composition", cmd);
		image::draw(&rt_main, param, cmd);
		rhi->endEvent(cmd);
		RenderPath2D::compose(cmd);
	}

	void RenderPath2D::compose(CommandList cmd)
	{
		image::Params params;
		params.enableFullScreen();
		params.blendFlag = enums::BLENDMODE_PREMULTIPLIED;
		image::draw(&getRenderResult(), params, cmd);
		RenderPath::compose(cmd);
	}

	void RenderPath2D::resizeBuffers()
	{
		RenderPath::resizeBuffers();
		RHI* rhi = qyhs::rhi::getRHI();
		{
			TextureDesc desc;
			desc.bind_flags = BindFlag::SHADER_RESOURCE | BindFlag::RENDER_TARGET;
			desc.format = Format::R8G8B8A8_UNORM;
			desc.width = getPhysicalWidth();
			desc.height = getPhysicalHeight();
			rhi->createTexture(&desc, &rt_final);
			rhi->setName(&rt_final, "rt_final");
		}
	}

	void RenderPath2D::update(float delta_time)
	{
		RenderPath::update(delta_time);
		GetGUI().update(*this, delta_time);
	}

}
