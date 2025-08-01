#include "translator.h"
#include "core/event/event_handler.h"


namespace qyhs
{
	namespace translator
	{
		PipelineState pso_solidpart;

		const float origin_size = 0.2f;
		const float axis_length = 3.5f;
		const float plane_min = 0.5f;
		const float plane_max = 1.5f;
		const float circle_radius = axis_length;
		const float circle_width = 1;
		const float circle2_radius = circle_radius + 0.7f;
		const float circle2_width = 0.3f;
		struct Vertex
		{
			XMFLOAT4 position;
			XMFLOAT4 color;
		};
		void LoadShaders()
		{
			RHI* rhi = rhi::getRHI();

			{
				PipelineStateDesc desc;

				desc.vertex_shader = renderer::GetShader(enums::VSTYPE_VERTEXCOLOR);
				desc.fragment_shader = renderer::GetShader(enums::PSTYPE_VERTEXCOLOR);
				desc.input_layout = renderer::GetInputLayout(enums::ILTYPE_VERTEXCOLOR);
				desc.depth_stencil_state = renderer::GetDepthStencilState(enums::DSSTYPE_DEFAULT);
				desc.rasterizer_state = renderer::GetRasterizerState(enums::RASTERIZER_STATE_TYPE_DOUBLE_SIDE);
				desc.blend_state = renderer::GetBlendState(enums::BLEND_STATE_TYPE_TRANSPARENT);
				desc.primitive_topology = PrimitiveTopology::TRIANGLE_LIST;

				rhi->createPipelineState(&desc, &pso_solidpart);
			}
		}
	}
	using namespace translator;
	void Translator::Draw(const scene::CameraComponent& camera, CommandList cmd)
	{
		if (selected.empty())
		{
			transform.ClearTransform();
			transform.updateTransform();
			return;
		}

		static bool shaders_loaded = false;
		if (!shaders_loaded)
		{
			shaders_loaded = true;
			eventhandler::subscribe(eventhandler::EVENT_RELOAD_SHADERS, [](uint64_t userdata) {translator::LoadShaders(); });
			translator::LoadShaders();
		}
		RHI* rhi = rhi::getRHI();
		rhi->beginEvent("Translator", cmd);
		//Axis
		{
			rhi->bindPipelineState(&pso_solidpart, cmd);
			uint32_t vertex_count = 0;
			RHI::GPUAllocation mem;



			const uint32_t segment_count = 18;
			const uint32_t cylinder_triangleCount = segment_count * 2;
			const uint32_t cone_triangleCount = cylinder_triangleCount;

			vertex_count = (cone_triangleCount + cylinder_triangleCount) * 3;
			mem = rhi->allocateGPU(vertex_count * sizeof(Vertex), cmd);
			const float cone_length = 0.75f;
			float cylinder_length = axis_length;

			cylinder_length -= cone_length;

			uint8_t* dst = (uint8_t*)mem.data;
			for (uint32_t i = 0; i < segment_count; ++i)
			{
				const float angle0 = (float)i / (float)segment_count * XM_2PI;
				const float angle1 = (float)(i + 1) / (float)segment_count * XM_2PI;
				// cylinder base:
				{
					const float cylinder_radius = 0.075f;
					const Vertex verts[] = {
						{XMFLOAT4(origin_size, std::sin(angle0) * cylinder_radius, std::cos(angle0) * cylinder_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(origin_size, std::sin(angle1) * cylinder_radius, std::cos(angle1) * cylinder_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle0) * cylinder_radius, std::cos(angle0) * cylinder_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle0) * cylinder_radius, std::cos(angle0) * cylinder_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle1) * cylinder_radius, std::cos(angle1) * cylinder_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(origin_size, std::sin(angle1) * cylinder_radius, std::cos(angle1) * cylinder_radius, 1), XMFLOAT4(1,1,1,1)},
					};
					std::memcpy(dst, verts, sizeof(verts));
					dst += sizeof(verts);
				}
				if (isTranslator)
				{
					// cone cap:
					const float cone_radius = origin_size;
					const Vertex verts[] = {
						{XMFLOAT4(cylinder_length, 0, 0, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle0) * cone_radius, std::cos(angle0) * cone_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle1) * cone_radius, std::cos(angle1) * cone_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(axis_length, 0, 0, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle0) * cone_radius, std::cos(angle0) * cone_radius, 1), XMFLOAT4(1,1,1,1)},
						{XMFLOAT4(cylinder_length, std::sin(angle1) * cone_radius, std::cos(angle1) * cone_radius, 1), XMFLOAT4(1,1,1,1)},
					};
					std::memcpy(dst, verts, sizeof(verts));
					dst += sizeof(verts);
				}
			}

			const GPUBuffer* vbs[] = { &mem.buffer };
			const uint32_t strides[] = {
			sizeof(Vertex),
			};
			const uint64_t offsets[] = {
				mem.offset,
			};
			rhi->bindVertexBuffers(vbs, 0, arraysize(vbs), strides, offsets, cmd);
			XMMATRIX VP = XMMatrixTranspose(camera.getViewProjection());
			MiscConstantBuffer sb;
			
			XMFLOAT3 test = {
				0.0f, 0.0f, 0.0f
			};
			
			
			XMMATRIX mat = XMMatrixScaling(dist, dist, dist) * XMMatrixTranslationFromVector(transform.GetPositionV()) * VP;
			XMMATRIX matX = XMMatrixIdentity();
			XMMATRIX matY = XMMatrixRotationZ(XM_PIDIV2) * XMMatrixRotationY(XM_PIDIV2);
			XMMATRIX matZ = XMMatrixRotationY(-XM_PIDIV2) * XMMatrixRotationZ(-XM_PIDIV2);

			const float channel_min = 0.25f; // min color channel, to avoid pure red/green/blue
			const XMFLOAT4 highlight_color = XMFLOAT4(1, 0.6f, 0, 1);

			// x
			XMStoreFloat4x4(&sb.g_xTransform, matX * mat);
			sb.g_xColor = state == TRANSLATOR_X ? highlight_color : XMFLOAT4(1, channel_min, channel_min, 1);
			sb.g_xColor.w *= opacity;
			rhi->bindDynamicConstantBuffer(sb, CBSLOT_RENDERER_MISC, cmd);
			rhi->draw(vertex_count, 0, cmd);

			// y
			XMStoreFloat4x4(&sb.g_xTransform, matY * mat);
			sb.g_xColor = state == TRANSLATOR_Y ? highlight_color : XMFLOAT4(channel_min, 1, channel_min, 1);
			sb.g_xColor.w *= opacity;
			rhi->bindDynamicConstantBuffer(sb, CBSLOT_RENDERER_MISC, cmd);
			rhi->draw(vertex_count, 0, cmd);

			// z
			XMStoreFloat4x4(&sb.g_xTransform, matZ * mat);
			sb.g_xColor = state == TRANSLATOR_Z ? highlight_color : XMFLOAT4(channel_min, channel_min, 1, 1);
			sb.g_xColor.w *= opacity;
			rhi->bindDynamicConstantBuffer(sb, CBSLOT_RENDERER_MISC, cmd);
			rhi->draw(vertex_count, 0, cmd);
		}
		rhi->endEvent(cmd);
	}
}