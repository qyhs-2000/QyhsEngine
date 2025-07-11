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
	bool show_bone = false;
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

		scene.update(delta_time);

		//Frustum culling for main camera
		visibility_main.layer_mask = layer_mask;
		visibility_main.scene = &scene;
		visibility_main.camera = camera;
		visibility_main.flags = renderer::Visibility::ALLOW_EVERYTHING;
		if (!occlusion_culling_enabled)
		{
			visibility_main.flags &= ~renderer::Visibility::ALLOW_OCCLUSION_CULLING;
		}
		renderer::updateVisibility(visibility_main);
		renderer::updatePerFrameData(scene, visibility_main, frame_cb);

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
			if (checkInput(EditorActions::MOVE_CAMERA_FORWARD)) { moveNew += XMVectorSet(0, 0, 1, 0); }
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
		std::shared_ptr<jobsystem::context> ctx = std::make_shared<jobsystem::context>();
		RHI* rhi = rhi::getRHI();
		CommandList cmd = rhi->beginCommandList();
		const scene::Scene& scene = getCurrentScene();
		CommandList cmd_prepareframe = cmd;
		//prepare the frame
		jobsystem::Execute(*ctx, [this, cmd](jobsystem::JobArgs args) {
			renderer::updateRenderData(visibility_main, frame_cb, cmd);
			});

		jobsystem::Wait(*ctx);
		rhi->waitQueue(cmd, RHI::QueueType::QUEUE_COMPUTE);
		static const uint32_t drawscene_flags = renderer::DRAWSCENE_OPAQUE | renderer::DRAWSCENE_MAINCAMERA;

		//main camera depth prepass
		cmd = rhi->beginCommandList();
		CommandList cmd_maincamera_prepass = cmd;
		rhi->waitCommandList(cmd, cmd_prepareframe);
		jobsystem::Execute(*ctx, [this, cmd](jobsystem::JobArgs args) {
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

		//jobsystem::Wait(*ctx);

		//main camera opaque color pass
		cmd = rhi->beginCommandList();
		rhi->waitCommandList(cmd, cmd_maincamera_prepass);
		jobsystem::Execute(*ctx, [this, cmd](jobsystem::JobArgs args) {
			renderer::bindCameraConstantBuffer(*camera, cmd);
			renderer::bindCommonResources(cmd);
			renderOpaques(cmd);
			});
		jobsystem::Wait(*ctx);
		//transparents
		jobsystem::Execute(*ctx, [this, cmd](jobsystem::JobArgs args) {
			renderer::bindCameraConstantBuffer(*camera, cmd);
			renderer::bindCommonResources(cmd);
			renderTransparents(cmd);
			});

		jobsystem::Wait(*ctx);
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
		//const scene::CameraComponent& camera = scene::getCamera();
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

			renderer::drawScene(visibility_main, renderer::DRAWSCENE_TRANSPARENT | renderer::DRAWSCENE_MAINCAMERA, RENDERPASS_MAIN, cmd);
		}
		renderer::drawDebugWorld(*camera, cmd);

		if(show_bone)
		{
			static PipelineState pso;
			if (!pso.isValid())
			{
				static auto LoadShaders = [] {
					PipelineStateDesc desc;
					desc.vertex_shader = renderer::GetShader(enums::VSTYPE_VERTEXCOLOR);
					desc.fragment_shader = renderer::GetShader(enums::PSTYPE_VERTEXCOLOR);
					desc.input_layout = renderer::GetInputLayout(enums::ILTYPE_VERTEXCOLOR);
					desc.depth_stencil_state = renderer::GetDepthStencilState(enums::DSSTYPE_DEPTHDISABLED);
					desc.rasterizer_state = renderer::GetRasterizerState(enums::RASTERIZER_STATE_TYPE_DOUBLE_SIDE);
					desc.blend_state = renderer::GetBlendState(enums::BLEND_STATE_TYPE_TRANSPARENT);
					desc.primitive_topology = PrimitiveTopology::TRIANGLE_LIST;
					rhi::getRHI()->createPipelineState(&desc, &pso);
					};
				
				LoadShaders();
			}

			size_t bone_count = 0;
			for (size_t i = 0; i < scene.armatures.getCount(); ++i)
			{
				const scene::ArmatureComponent& armature = scene.armatures[i];
				bone_count += armature.bone_collection.size();
			}

			if (bone_count > 0)
			{
				struct Vertex
				{
					XMFLOAT4 position;
					XMFLOAT4 color;
				};
				const size_t segment_count = 18 + 1 + 18 + 1;
				const size_t vb_size = sizeof(Vertex) * (bone_count * (segment_count + 1 + 1));
				const size_t ib_size = sizeof(uint32_t) * bone_count * (segment_count + 1) * 3;
				RHI::GPUAllocation mem = rhi->allocateGPU(vb_size + ib_size, cmd);
				Vertex* vertices = (Vertex*)mem.data;
				uint32_t* indices = (uint32_t*)((uint8_t*)mem.data + vb_size);
				uint32_t vertex_count = 0;
				uint32_t index_count = 0;

				const XMVECTOR Eye = camera->GetEye();
				const XMVECTOR Unit = XMVectorSet(0, 1, 0, 0);

				for (size_t i = 0; i < scene.armatures.getCount(); ++i)
				{
					const scene::ArmatureComponent& armature = scene.armatures[i];
					for (ecs::Entity entity : armature.bone_collection)
					{
						if (!scene.transforms.contain(entity))
							continue;
						const scene::TransformComponent& transform = *scene.transforms.getComponent(entity);
						XMVECTOR a = transform.GetPositionV();
						XMVECTOR b = a + XMVectorSet(0, 0.1f, 0, 0);
						
						
						{
							// Search for child to connect bone tip:
							bool child_found = false;
							
							if (!child_found)
							{
								for (ecs::Entity child : armature.bone_collection)
								{
									const scene::HierarchyComponent* hierarchy = scene.hierarchy.getComponent(child);
									if (hierarchy != nullptr && hierarchy->parent_id == entity && scene.transforms.contain(child))
									{
										const scene::TransformComponent& child_transform = *scene.transforms.getComponent(child);
										b = child_transform.GetPositionV();
										child_found = true;
										break;
									}
								}
							}
							if (!child_found)
							{
								// No child, try to guess bone tip compared to parent (if it has parent):
								const scene::HierarchyComponent* hierarchy = scene.hierarchy.getComponent(entity);
								if (hierarchy != nullptr && scene.transforms.contain(hierarchy->parent_id))
								{
									const scene::TransformComponent& parent_transform = *scene.transforms.getComponent(hierarchy->parent_id);
									XMVECTOR ab = a - parent_transform.GetPositionV();
									b = a + ab;
								}
							}
						}
						XMVECTOR ab = XMVector3Normalize(b - a);

						primitive::Capsule capsule;
						capsule.radius = math::Distance(a, b) * 0.1f;

						a -= XMVectorMultiply(ab, XMVectorReplicate(capsule.radius));  
						b += XMVectorMultiply(ab, XMVectorReplicate(capsule.radius));  

						XMStoreFloat3(&capsule.base, a);
						XMStoreFloat3(&capsule.tip, b);
						XMFLOAT4 color = inactiveEntityColor;

						

						color.w = 0.6;

						XMVECTOR Base = XMLoadFloat3(&capsule.base);
						XMVECTOR Tip = XMLoadFloat3(&capsule.tip);
						XMVECTOR Radius = XMVectorReplicate(capsule.radius);
						XMVECTOR Normal = XMVector3Normalize(Tip - Base);
						XMVECTOR Tangent = XMVector3Normalize(XMVector3Cross(Normal, Base - Eye));
						XMVECTOR Binormal = XMVector3Normalize(XMVector3Cross(Tangent, Normal));

						XMVECTOR LineEndOffset = XMVectorMultiply(Normal, Radius);  
						XMVECTOR A = XMVectorAdd(Base, LineEndOffset);  
						XMVECTOR B = XMVectorSubtract(Tip, LineEndOffset);  
						XMVECTOR AB = XMVectorMultiply(Unit, XMVector3Length(B - A));  

						XMMATRIX M = { Tangent,Normal,Binormal,XMVectorSetW(A, 1) };

						uint32_t center_vertex_index = vertex_count;
						Vertex center_vertex;
						XMStoreFloat4(&center_vertex.position, A);
						center_vertex.position.w = 1;
						center_vertex.color = color;
						center_vertex.color.w = 0;
						std::memcpy(vertices + vertex_count, &center_vertex, sizeof(center_vertex));
						vertex_count++;

						for (size_t i = 0; i < segment_count; ++i)
						{
							XMVECTOR segment_pos;
							const float angle0 = XM_PIDIV2 + (float)i / (float)segment_count * XM_2PI;
							if (i < 18)
							{
								segment_pos = XMVectorSet(sinf(angle0) * capsule.radius, cosf(angle0) * capsule.radius, 0, 1);
							}
							else if (i == 18)
							{
								segment_pos = XMVectorSet(sinf(angle0) * capsule.radius, cosf(angle0) * capsule.radius, 0, 1);
							}
							else if (i > 18 && i < 18 + 1 + 18)
							{
								segment_pos = AB + XMVectorSet(sinf(angle0) * capsule.radius * 0.5f, cosf(angle0) * capsule.radius * 0.5f, 0, 1);
							}
							else
							{
								segment_pos = AB + XMVectorSet(sinf(angle0) * capsule.radius * 0.5f, cosf(angle0) * capsule.radius * 0.5f, 0, 1);
							}
							segment_pos = XMVector3Transform(segment_pos, M);

							Vertex vertex;
							XMStoreFloat4(&vertex.position, segment_pos);
							vertex.position.w = 1;
							vertex.color = color;
							//vertex.color.w = 0;
							std::memcpy(vertices + vertex_count, &vertex, sizeof(vertex));
							uint32_t ind[] = { center_vertex_index,vertex_count - 1,vertex_count };
							std::memcpy(indices + index_count, ind, sizeof(ind));
							index_count += arraysize(ind);
							vertex_count++;
						}
						// closing triangle fan:
						uint32_t ind[] = { center_vertex_index,vertex_count - 1,center_vertex_index+1 };
						std::memcpy(indices + index_count, ind, sizeof(ind));
						index_count += arraysize(ind);
					}
				}

				rhi->beginEvent("Bone capsules", cmd);
				rhi->bindPipelineState(&pso, cmd);

				const GPUBuffer* vbs[] = {
					&mem.buffer,
				};
				const uint32_t strides[] = {
					sizeof(Vertex)
				};
				const uint64_t offsets[] = {
					mem.offset,
				};
				rhi->bindVertexBuffers(vbs, 0, arraysize(vbs), strides, offsets, cmd);
				rhi->bindIndexBuffer(&mem.buffer, IndexBufferFormat::UINT32, mem.offset + vb_size, cmd);

				rhi->drawIndexed(index_count, 0, 0, cmd);
				rhi->endEvent(cmd);
			}
		}

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

			renderer::drawScene(visibility_main, renderer::DRAWSCENE_OPAQUE | renderer::DRAWSCENE_MAINCAMERA, RENDERPASS_MAIN, cmd);
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
