#include "renderer.h"
#include "core/helper.h"
#include "function/render/shader_compiler.h"
#include <shader/shader_interop.h>
#include <shader/shader_interop_renderer.h>
#include "scene.h"
#include "core/jobsystem.h"
#include <iostream>
using namespace qyhs::enums;
//#include "function/render/scene_component.h"

union ObjectRenderingVariant
{
	struct
	{
		uint32_t renderpass : 4;	// wi::enums::RENDERPASS
		uint32_t shadertype : 8;	// MaterialComponent::SHADERTYPE
		uint32_t blendmode : 4;		// wi::enums::BLENDMODE
		uint32_t cullmode : 2;		// wi::graphics::CullMode
		uint32_t tessellation : 1;	// bool
		uint32_t alphatest : 1;		// bool
		uint32_t sample_count : 4;	// 1, 2, 4, 8
		uint32_t mesh_shader : 1;	// bool
	} bits;
	uint32_t value;
};
static_assert(sizeof(ObjectRenderingVariant) == sizeof(uint32_t));
enum OBJECT_MESH_SHADER_PSO
{
	OBJECT_MESH_SHADER_PSO_DISABLE,
	OBJECT_MESH_SHADER_PSO_ENABLE,
	OBJECT_MESH_SHADER_PSO_COUNT
};
bool occlusion_culling_enabled = true;
inline PipelineState* getObjectPSO(ObjectRenderingVariant variant)
{
	static std::unordered_map<uint32_t, PipelineState> object_pso[RENDERPASS_COUNT][qyhs::scene::MaterialComponent::SHADERTYPE_COUNT][OBJECT_MESH_SHADER_PSO_COUNT];
	return &object_pso[variant.bits.renderpass][variant.bits.shadertype][variant.bits.mesh_shader][variant.value];
}

qyhs::jobsystem::Context object_pso_job_ctxs[RENDERPASS_COUNT][OBJECT_MESH_SHADER_PSO_COUNT];
namespace qyhs::renderer
{
	Shader shaders[SHADER_TYPE_COUNT];
	RHI*& rhi = rhi::getRHI();
	std::string SHADER_PATH = helper::getCurrentPath() + "/shaders/";
	std::atomic<size_t> SHADER_MISSING{ 0 };
	GPUBuffer frame_constant_buffer;
	RasterizerState rasterizers[RASTERIZER_STATE_TYPE_COUNT];
	BlendState blend_states[BLEND_STATE_TYPE_COUNT];
	DepthStencilState depth_stencils[DSSTYPE_COUNT];
	bool loadShader(graphics::ShaderStage stage, graphics::Shader& shader, const std::string& filename,ShaderModel min_shader_model,const std::vector<std::string>& permutation_definitions)
	{
		std::string shader_binary_filename = SHADER_PATH + filename;
		if (!permutation_definitions.empty())
		{
			std::string ext = helper::getExtensionFromFileName(shader_binary_filename);
			shader_binary_filename = helper::removeExtension(shader_binary_filename);
			for (auto& def : permutation_definitions)
			{
				shader_binary_filename += "_" + def;
			}
			shader_binary_filename += "." + ext;
		}

		if (shadercompiler::isShaderBinaryFileOutDatad(shader_binary_filename))
		{
			std::string shader_source_dir = SHADER_SOURCE_DIR;
			qyhs::shadercompiler::CompilerInput input;
			input.format = rhi->getShaderFormat();
			input.stage = stage;
			input.minshadermodel = min_shader_model;
			helper::makeAbsolutePath(shader_source_dir);
			input.include_directories.push_back(shader_source_dir);
			input.include_directories.push_back(shader_source_dir + helper::getDirectoryFromPath(filename));
			input.shader_source_filename = helper::replaceExtension(shader_source_dir + filename, "hlsl");

			qyhs::shadercompiler::CompilerOutput output;
			qyhs::shadercompiler::compile(input, output);

			if (output.isValid())
			{
				bool save_success = qyhs::shadercompiler::saveShaderAndMetaData(shader_binary_filename, output);
				assert(save_success == 1);
				return rhi->createShader(input.stage, output.shader_data, output.shader_size, &shader);
			}
			else
			{
				assert(0);
			}
			if (rhi != nullptr)
			{
				std::vector<uint8_t> buffer;
				if (qyhs::helper::readFile(shader_binary_filename, buffer))
				{
					bool success = rhi->createShader(stage, buffer.data(), buffer.size(), &shader);
					if (success)
					{
						//rhi->setName(&shader, shaderbinaryfilename.c_str());
					}
					return success;
				}
				else
				{
					SHADER_MISSING.fetch_add(1);
				}
			}

			return false;
		}
	}

	struct instancedBatch
	{

	};

	struct RenderBatch
	{
		uint32_t mesh_index;
		uint32_t instance_index;
		inline uint32_t getInstanceIndex()const { return instance_index; }
	};

	struct RenderQueue
	{
		std::vector<RenderBatch> render_batches;
		void init()
		{
			render_batches.clear();
		}
		bool empty()const { return render_batches.empty(); }
		size_t size()const { return render_batches.size(); }
		void add(uint32_t mesh_index, uint32_t instance_index)
		{
			RenderBatch& render_obj = render_batches.emplace_back();
			render_obj.mesh_index = mesh_index;
			render_obj.instance_index = instance_index;
		}
	};



	void renderMeshes(scene::Scene* scene, const RenderQueue& render_queue,RENDERPASS render_pass, CommandList cmd)
	{
		if (render_queue.empty())
		{
			return;
		}
		RHI* rhi = rhi::getRHI();
		struct InstanceBatch
		{
			uint32_t mesh_index = ~0u;
			uint32_t instance_count = 0;
			uint32_t lod = 0;
			uint32_t data_offset = 0;
		} instanced_batch;

		RenderPassInfo render_pass_info = rhi->getRenderPassInfo(cmd);

		rhi->beginEvent("Render Meshes", cmd);
		const size_t allocate_size = render_queue.size() * sizeof(ShaderMeshInstancePointer);
		RHI::GPUAllocation instances = rhi->allocateGPU(allocate_size, cmd);
		int instance_buffer_descriptor_index = rhi->getDescriptorIndex(&instances.buffer, SubresourceType::SRV);
		auto batch_flush = [&]()
			{
				if (instanced_batch.instance_count == 0)
				{
					return;
				}
				scene::MeshComponent& mesh = scene->meshes[instanced_batch.mesh_index];
				if (!mesh.general_buffer.isValid())
				{
					return;
				}

				int first_subset = 0;
				int last_subset = 0;
				mesh.getLodSubsetRange(instanced_batch.lod, first_subset, last_subset);
				for (uint32_t subset_index = first_subset; subset_index < last_subset; ++subset_index)
				{
					const scene::MeshComponent::MeshSubset& subset = mesh.subsets[subset_index];

					rhi->bindIndexBuffer(&mesh.general_buffer, mesh.getIndexFormat(), mesh.ib.offset,cmd);

					ObjectPushConstant push_constant;
					push_constant.geometry_index = mesh.geometry_offset + subset_index;
					push_constant.material_index = subset.material_index;
					push_constant.instances = instance_buffer_descriptor_index;
					push_constant.instance_offset = instanced_batch.data_offset;
					const scene::MaterialComponent& material = scene->materials[subset.material_index];
					const PipelineState* pso = nullptr;
					{
						ObjectRenderingVariant variant = {};
						variant.bits.renderpass = render_pass;
						variant.bits.shadertype = material.shader_type;
						variant.bits.blendmode = material.getBlendMode();
						mesh.setDoubleSided(true);
						variant.bits.cullmode = (mesh.isDoubleSided() || material.isDoubleSided()) ? (uint32_t)CullMode::NONE : (uint32_t)CullMode::BACK;
						variant.bits.tessellation = 0;
						variant.bits.alphatest = 0;
						variant.bits.sample_count = render_pass_info.sample_count;
						variant.bits.mesh_shader = 0;
						pso = getObjectPSO(variant);
						assert(pso->isValid());
					}
					rhi->bindPipelineState(pso, cmd);
					rhi->pushConstants(&push_constant, sizeof(push_constant), cmd);
					rhi->drawIndexedInstanced(subset.index_count, instanced_batch.instance_count, subset.index_offset, 0, 0, cmd);
				}

			};
		uint32_t instance_count = 0;
		for (auto& render_batch : render_queue.render_batches)
		{
			const uint32_t mesh_index = render_batch.mesh_index;
			uint32_t instance_index = render_batch.getInstanceIndex();
			scene::ObjectComponent& instance = scene->objects[instance_index];

			if (mesh_index != instanced_batch.mesh_index)
			{
				batch_flush();

				instanced_batch = {};
				instanced_batch.mesh_index = mesh_index;
				instanced_batch.instance_count = 0;
				instanced_batch.data_offset = (uint32_t)(instances.offset + sizeof(ShaderMeshInstancePointer) * instance_count);
				instanced_batch.lod = instance.lod;
			}
			ShaderMeshInstancePointer instance_pointer;
			instance_pointer.create(instance_index);
			std::memcpy((ShaderMeshInstance*)instances.data + instance_count, &instance_pointer, sizeof(instance_pointer));
			++instanced_batch.instance_count;
			++instance_count;
		}
		batch_flush();

		rhi->endEvent(cmd);
	}

	void drawScene(const Visibility& visibility, uint32_t flags,RENDERPASS render_pass, CommandList cmd)
	{
		static thread_local RenderQueue render_queue;
		render_queue.init();
		const bool occlusion = flags & DRAWSCENE_OPAQUE;
		for (uint32_t instance_id : visibility.visible_objects)
		{
			if (occlusion && visibility.scene->occlusion_result_objects[instance_id].isOccluded())
			{
				continue;
			}
			const scene::ObjectComponent& object = visibility.scene->objects[instance_id];
			render_queue.add(object.mesh_index, instance_id);
		}
		if (!render_queue.empty())
		{
			renderMeshes(visibility.scene, render_queue,render_pass, cmd);
		}

	}

	void bindCameraConstantBuffer(scene::CameraComponent& camera, CommandList cmd)
	{
		CameraConstantBuffer constant_buffer;
		constant_buffer.init();

		ShaderCamera& shader_camera = constant_buffer.cameras[0];
		shader_camera.options = camera.shadercamera_options;
		if (camera.isOrtho())
		{
			shader_camera.options |= SHADERCAMERA_OPTION_ORTHO;
		}
		XMMATRIX inv_view_projection = camera.getInvViewProjection();
		XMStoreFloat4x4(&shader_camera.view_projection, camera.getViewProjection());
		XMStoreFloat4x4(&shader_camera.view, camera.getView());
		XMStoreFloat4x4(&shader_camera.projection, camera.getProjection());
		shader_camera.position = camera.eye;
		shader_camera.distant_from_origin = XMVectorGetX(XMVector3Length(XMLoadFloat3(&shader_camera.position)));
		XMStoreFloat4x4(&shader_camera.inverse_view, camera.getInvView());
		XMStoreFloat4x4(&shader_camera.inverse_projection, camera.getInvProjection());
		XMStoreFloat4x4(&shader_camera.inverse_view_projection, inv_view_projection);
		shader_camera.forward = camera.look_at;
		shader_camera.up = camera.up;
		shader_camera.z_near = std::max((float)0.01, camera.z_near);
		shader_camera.z_far = std::min(camera.z_far, 5000.f);

		rhi->bindDynamicConstantBuffer(constant_buffer, CBSLOT_RENDERER_CAMERA, cmd);
	}

	void bindCommonResources(CommandList cmd)
	{
		RHI* rhi = rhi::getRHI();
		rhi->bindConstantBuffer(&frame_constant_buffer, CBSLOT_RENDERER_FRAME, cmd);

	}

	void updateVisibility(Visibility& visibility)
	{
		jobsystem::Context ctx;
		visibility.frustum = visibility.camera->frustum;
		static constexpr uint32_t group_size = 64;
		struct StreamCompaction
		{
			uint8_t list[group_size];
			uint8_t count;
		};
		static constexpr size_t shared_memory_size = sizeof(StreamCompaction);
		visibility.clear();
		if (visibility.flags & Visibility::ALLOW_OBJECTS)
		{
			//cull objects
			const uint32_t object_size = (uint32_t)std::min((uint32_t)visibility.scene->aabb_objects.size(), visibility.scene->objects.getCount());
			visibility.visible_objects.resize(object_size);
			jobsystem::dispatch(ctx, object_size, group_size, [&](jobsystem::JobArgs args) {
				StreamCompaction& stream_compaction = *(StreamCompaction*)args.shared_memory;
				if (args.is_first_job_in_group)
				{
					stream_compaction.count = 0;
				}
				const primitive::AABB& aabb = visibility.scene->aabb_objects[args.job_index];
				if (visibility.frustum.checkBox(aabb))
				{
					stream_compaction.list[stream_compaction.count++] = args.group_index;
					const scene::ObjectComponent& object = visibility.scene->objects[args.job_index];
					//Scene
				}
				// Global stream compaction:
				if (args.is_last_job_in_group && stream_compaction.count > 0)
				{
					uint32_t prev_count = visibility.object_counter.fetch_add(stream_compaction.count);
					uint32_t groupOffset = args.group_id * group_size;
					for (uint32_t i = 0; i < stream_compaction.count; ++i)
					{
						visibility.visible_objects[prev_count + i] = groupOffset + stream_compaction.list[i];
					}
				}

				}, shared_memory_size
			);
		}
	}

	bool getOcclusionCullingEnabled()
	{
		return occlusion_culling_enabled;
	}

	SHADER_TYPE getVertexShaderType(RENDERPASS render_pass, bool tessellation, bool alpha_test, bool transparency)
	{
		SHADER_TYPE vs_type = SHADER_TYPE_COUNT;
		switch (render_pass)
		{
		case qyhs::enums::RENDERPASS_MAIN:
			if (tessellation)
			{
				vs_type = VS_OBJECT_TESSELLATION_VS;
			}
			else
			{
				vs_type = VS_OBJECT_COMMON_VS;
			}
			break;
		/*case RENDERPASS_PREPASS:
		case qyhs::enums::RENDERPASS_PREPASS_DEPTHONLY:
			if (tessellation)
			{
				if (alpha_test)
				{
					vs_type = VS_OBJECT_PREPASS_ALPHATEST_TESSELLATION;
				}
				else
				{
					vs_type = VS_OBJECT_PREPASS_TESSELLATION;
				}
			}
			else
			{
				if (alpha_test)
				{
					vs_type = VSTYPE_OBJECT_PREPASS_ALPHATEST;
				}
				else
				{
					vs_type = VSTYPE_OBJECT_PREPASS;
				}
			}
			break;*/
		}
		return vs_type;
	}

	SHADER_TYPE getPixelShaderType(RENDERPASS renderPass, bool alphatest, bool transparent, scene::MaterialComponent::SHADERTYPE shaderType)
	{
		SHADER_TYPE realPS = SHADER_TYPE_COUNT;

		switch (renderPass)
		{
		case RENDERPASS_MAIN:
			realPS = SHADER_TYPE((transparent ? PSTYPE_OBJECT_TRANSPARENT_PERMUTATION_BEGIN : PSTYPE_OBJECT_PERMUTATION_BEGIN) + shaderType);
			break;
		/*case RENDERPASS_PREPASS:
			if (alphatest)
			{
				realPS = PSTYPE_OBJECT_PREPASS_ALPHATEST;
			}
			else
			{
				realPS = PSTYPE_OBJECT_PREPASS;
			}
			break;
		case RENDERPASS_PREPASS_DEPTHONLY:
			if (alphatest)
			{
				realPS = PSTYPE_OBJECT_PREPASS_DEPTHONLY_ALPHATEST;
			}
			else
			{
				realPS = PSTYPE_OBJECT_PREPASS_DEPTHONLY;
			}
			break;*/
		}

		return realPS;
	}

	void loadShaders()
	{
		RHI* rhi = rhi::getRHI();
		jobsystem::Context ctx;
		jobsystem::execute(ctx, [](jobsystem::JobArgs args)
			{
				loadShader(ShaderStage::VERTEX_SHADER, shaders[VS_OBJECT_COMMON_VS], "object_common_vs.cso");

			});
		jobsystem::dispatch(ctx, scene::MaterialComponent::SHADERTYPE_COUNT, 1, [](jobsystem::JobArgs args) {
			loadShader(ShaderStage::PIXEL_SHADER, shaders[PSTYPE_OBJECT_PERMUTATION_BEGIN + args.job_index],
				"objectPS.cso", ShaderModel::SM_6_0, scene::MaterialComponent::shadertype_definitions[args.job_index]);
			});
		jobsystem::wait(ctx);

		for (uint32_t render_pass = 0; render_pass < RENDERPASS_COUNT; ++render_pass)
		{
			for (uint32_t mesh_shader = 0; mesh_shader <= rhi->checkCapability(GraphicsDeviceCapability::MESH_SHADER) ? 1u : 0u; ++mesh_shader)
			{
				jobsystem::wait(object_pso_job_ctxs[render_pass][mesh_shader]);
				object_pso_job_ctxs[render_pass][mesh_shader].priority = jobsystem::Priority::Low;
				for (uint32_t shader_type = 0; shader_type < scene::MaterialComponent::SHADERTYPE_COUNT; ++shader_type)
				{
					jobsystem::execute(object_pso_job_ctxs[render_pass][mesh_shader], [=](jobsystem::JobArgs args) {
						//now just for opaque scene
						for (uint32_t blend_mode = 0; blend_mode <= BLENDMODE_OPAQUE; ++blend_mode)
						{
							for (uint32_t cull_mode = 0; cull_mode < (uint32_t)CullMode::COUNT; ++cull_mode)
							{
								//now just for no tessellation
								for (uint32_t tessellation = 0; tessellation < 1; ++tessellation)
								{
									if (tessellation && render_pass > RENDERPASS_MAIN)
									{
										continue;
									}
									for (uint32_t alpha_test = 0; alpha_test <= 1; ++alpha_test)
									{
										const bool transparency = blend_mode != BLENDMODE_OPAQUE;
										PipelineStateDesc desc;
										
										if (mesh_shader)
										{
											if (tessellation)
												continue;
										}
										else
										{
											SHADER_TYPE real_vertex_shader = getVertexShaderType((RENDERPASS)render_pass,  tessellation,  alpha_test,  transparency);
											desc.vertex_shader = real_vertex_shader<SHADER_TYPE_COUNT?&shaders[real_vertex_shader]:nullptr;
										}
										SHADER_TYPE real_fragment_shader = getPixelShaderType((RENDERPASS)render_pass,alpha_test,transparency,(scene::MaterialComponent::SHADERTYPE)shader_type);
										desc.fragment_shader = real_fragment_shader < SHADER_TYPE_COUNT ? &shaders[real_fragment_shader] : nullptr;

										ObjectRenderingVariant variant = {};
										variant.bits.renderpass = render_pass;
										variant.bits.shadertype = shader_type;
										variant.bits.blendmode = blend_mode;
										variant.bits.cullmode = cull_mode;
										variant.bits.tessellation = tessellation;
										variant.bits.alphatest = alpha_test;
										variant.bits.sample_count = 1;
										variant.bits.mesh_shader = mesh_shader;

										switch ((CullMode)cull_mode)
										{
										default:
										case CullMode::BACK:
											desc.rasterizer_state = &rasterizers[RASTERIZER_STATE_TYPE_FRONT];
											break;
										case CullMode::FRONT:
											desc.rasterizer_state = &rasterizers[RASTERIZER_STATE_TYPE_BACK];
											break;
										case CullMode::NONE:
											desc.rasterizer_state = &rasterizers[RASTERIZER_STATE_TYPE_DOUBLE_SIDE];
											break;
										}

										if (tessellation)
										{
											desc.primitive_topology = PrimitiveTopology::PATCH_LIST;
										}
										else
										{
											desc.primitive_topology = PrimitiveTopology::TRIANGLE_LIST;
										}

										switch (blend_mode)
										{
										case BLENDMODE_OPAQUE:
											desc.blend_state = &blend_states[BLEND_STATE_TYPE_OPAQUE];
											break;
										case BLENDMODE_ALPHA:
											desc.blend_state = &blend_states[BLEND_STATE_TYPE_ALPHA];
											break;
										/*
										case BLENDMODE_ADDITIVE:
											desc.blend_state = &blend_states[BLEND_STATE_TYPE_ADDITIVE];
											break;
										case BLENDMODE_PREMULTIPLIED:
											desc.blend_state = &blend_states[BLEND_STATE_TYPE_PREMULTIPLIED];
											break;
										case BLENDMODE_MULTIPLY:
											desc.blend_state = &blend_states[BLEND_STATE_TYPE_MULTIPLY];
											break;*/
										default:
											assert(0);
											break;
										}

										switch (render_pass)
										{
										case RENDERPASS_MAIN:
										{
											RenderPassInfo render_pass_info{};
											render_pass_info.rt_count = 1;
											render_pass_info.rt_formats[0] = render_pass == RENDERPASS_MAIN ? format_rendertarget_main : format_idbuffer;
											render_pass_info.ds_format = format_depthbuffer_main;
											const uint32_t msaa_support[] = { 1,2,4,8 };
											for (uint32_t msaa : msaa_support)
											{
												variant.bits.sample_count = msaa;
												render_pass_info.sample_count = msaa;
												rhi->createPipelineState(&desc, getObjectPSO(variant), &render_pass_info);
											}
										}
										break;
										default:
											assert(0);
											break;
										}

									}
								}
							}
						}

						});
				}
			}
		}

	}

	//setup rasterizer state,blend state 
	void setupStates()
	{
		RasterizerState rasterizer_state;
		rasterizer_state.cull_mode = CullMode::BACK;
		rasterizer_state.fill_mode = FillMode::SOLID;
		rasterizer_state.front_counter_clockwise = true;
		rasterizer_state.depth_bias = 0;
		rasterizer_state.depth_bias_clamp = 0;
		rasterizer_state.slope_scaled_depth_bias = 0;
		rasterizer_state.depth_clip_enable = true;
		rasterizer_state.multisample_enable = false;
		rasterizer_state.antialiased_line_enable = false;
		rasterizer_state.conservative_rasterization_enable = false;
		rasterizers[RASTERIZER_STATE_TYPE_FRONT] = rasterizer_state;

		BlendState blend_state;
		blend_state.render_targets[0].blend_enable = false;
		blend_state.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		blend_state.alpha_to_coverage_enable = false;
		blend_state.independent_blend_enable = false;

		blend_state.render_targets[0].src_blend = Blend::SRC_ALPHA;
		blend_state.render_targets[0].dest_blend = Blend::ONE;
		blend_state.render_targets[0].blend_op = BlendOp::ADD;
		blend_state.render_targets[0].src_blend_alpha = Blend::ZERO;
		blend_state.render_targets[0].dest_blend_alpha = Blend::ONE;
		blend_state.render_targets[0].blend_op_alpha = BlendOp::ADD;
		blend_state.render_targets[0].render_target_write_mask = ColorWrite::ENABLE_ALL;
		blend_state.independent_blend_enable = false,
			blend_state.alpha_to_coverage_enable = false;
		blend_states[BLEND_STATE_TYPE_ADDITIVE] = blend_state;

		DepthStencilState dsd;
		dsd.depth_enable = true;
		dsd.depth_write_mask = DepthWriteMask::ALL;
		dsd.depth_func = ComparisonFunc::GREATER;

		dsd.stencil_enable = true;
		dsd.stencil_read_mask = 0;
		dsd.stencil_write_mask = 0xFF;
		dsd.front_face.stencil_func = ComparisonFunc::ALWAYS;
		dsd.front_face.stencil_pass_op = StencilOp::REPLACE;
		dsd.front_face.stencil_fail_op = StencilOp::KEEP;
		dsd.front_face.stencil_depth_fail_op = StencilOp::KEEP;
		dsd.back_face.stencil_func = ComparisonFunc::ALWAYS;
		dsd.back_face.stencil_pass_op = StencilOp::REPLACE;
		dsd.back_face.stencil_fail_op = StencilOp::KEEP;
		dsd.back_face.stencil_depth_fail_op = StencilOp::KEEP;
		depth_stencils[DSSTYPE_DEFAULT] = dsd;

		dsd.depth_write_mask = DepthWriteMask::ZERO;

		depth_stencils[DSSTYPE_HOLOGRAM] = dsd;
	}

	void loadBuffers()
	{
		RHI* rhi = rhi::getRHI();
		GPUBufferDesc desc{};
		desc.usage = Usage::DEFAULT;
		desc.size = sizeof(FrameConstantBuffer);
		desc.bind_flags = BindFlag::CONSTANT_BUFFER;
		rhi->createBuffer(&desc, &frame_constant_buffer);
		rhi->setName(&frame_constant_buffer, "frame_constant_buffer");
	}

	void initialize()
	{
		setupStates();
		loadBuffers();
		loadShaders();
	}

	void updatePerFrameData(scene::Scene& scene, Visibility& visibility, FrameConstantBuffer& frame_cb)
	{
		frame_cb.scene = visibility.scene->shader_scene;
	}

	void updateRenderData(const Visibility& visibility, const FrameConstantBuffer& frame_cb,CommandList & cmd)
	{
		rhi->updateBuffer(&frame_constant_buffer,&frame_cb,cmd);
		if (visibility.scene->instance_buffer.isValid() && visibility.scene->scene_instance_count > 0)
		{
			rhi->copyBuffer(&visibility.scene->instance_buffer, 0, 
				&visibility.scene->instance_upload_buffers[rhi->getBufferIndex()], 0,
				visibility.scene->scene_instance_count * sizeof(ShaderMeshInstance), cmd);
		}

		if (visibility.scene->material_buffer.isValid() && visibility.scene->materials_array_size> 0)
		{
			rhi->copyBuffer(&visibility.scene->material_buffer, 0, 
				&visibility.scene->material_upload_buffers[rhi->getBufferIndex()], 0,
				visibility.scene->materials_array_size * sizeof(ShaderMaterial), cmd);
		}

		if (visibility.scene->geometry_buffer.isValid() && visibility.scene->geometry_array_size> 0)
		{
			rhi->copyBuffer(&visibility.scene->geometry_buffer, 0, 
				&visibility.scene->geometry_upload_buffers[rhi->getBufferIndex()], 0,
				visibility.scene->geometry_array_size * sizeof(ShaderGeometry), cmd);
		}

		bindCommonResources(cmd);
	}


}