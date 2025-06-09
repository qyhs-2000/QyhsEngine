#pragma once
#include "function/render/rhi/rhi.h"
#include <string>
#include "function/render/scene_component.h"
#include "scene.h"
#include "function/render/primitive.h"
namespace qyhs::renderer
{
	enum DRAWSCENE_FLAGS
	{
		DRAWSCENE_OPAQUE = 1 << 0, // include opaque objects
		DRAWSCENE_TRANSPARENT = 1 << 1, // include transparent objects
		DRAWSCENE_OCCLUSIONCULLING = 1 << 2, // enable skipping objects based on occlusion culling results
		DRAWSCENE_TESSELLATION = 1 << 3, // enable tessellation
		DRAWSCENE_HAIRPARTICLE = 1 << 4, // include hair particles
		DRAWSCENE_IMPOSTOR = 1 << 5, // include impostors
		DRAWSCENE_OCEAN = 1 << 6, // include ocean
		DRAWSCENE_SKIP_PLANAR_REFLECTION_OBJECTS = 1 << 7, // don't draw subsets which have planar reflection material
		DRAWSCENE_FOREGROUND_ONLY = 1 << 8, // only include objects that are tagged as foreground
		DRAWSCENE_MAINCAMERA = 1 << 9, // If this is activate, then ObjectComponent with SetNotVisibleInMainCamera(true) won't be drawn
	};
	class Visibility
	{
	public:
		Visibility() {};
		enum FLAGS
		{
			EMPTY = 0,
			ALLOW_OBJECTS = 1 << 0,
			ALLOW_LIGHTS = 1 << 1,
			ALLOW_DECALS = 1 << 2,
			ALLOW_ENVPROBES = 1 << 3,
			ALLOW_EMITTERS = 1 << 4,
			ALLOW_HAIRS = 1 << 5,
			ALLOW_REQUEST_REFLECTION = 1 << 6,
			ALLOW_OCCLUSION_CULLING = 1 << 7,
			ALLOW_SHADOW_ATLAS_PACKING = 1 << 8,

			ALLOW_EVERYTHING = ~0u
		};
		uint32_t flags = EMPTY;
		uint32_t layer_mask = 0XFFFFFFFF;
		scene::Scene* scene;
		scene::CameraComponent* camera = nullptr;
		primitive::Frustum frustum;
		std::vector<uint32_t> visible_objects;
		std::atomic<uint32_t> object_counter{ 0 };
		void clear()
		{
			object_counter.store(0);
			visible_objects.clear();
		}
	private:
	};

	constexpr graphics::Format format_rendertarget_main = Format::R11G11B10_FLOAT;
	constexpr graphics::Format format_depthbuffer_main = Format::D32_FLOAT_S8X24_UINT;
	constexpr graphics::Format format_idbuffer = Format::R32_UINT;
	bool loadShader(graphics::ShaderStage stage, graphics::Shader& shader, const std::string& name, ShaderModel min_shader_model = ShaderModel::SM_6_0, const std::vector<std::string>& permutation_definitions = {});
	void drawScene(const Visibility & visibility, uint32_t flags,RENDERPASS render_pass, CommandList cmd);
	void bindCameraConstantBuffer(scene::CameraComponent& camera, CommandList cmd);
	void bindCommonResources(CommandList cmd);
	void updateVisibility(Visibility &visibility);
	bool getOcclusionCullingEnabled();
	void initialize();
	void updatePerFrameData(scene::Scene& scene, Visibility& visibility, FrameConstantBuffer& frame_cb);
	void updateRenderData(const Visibility& visibility, const FrameConstantBuffer& frame_cb,CommandList  cmd);
	const Sampler* getSampler(enums::SAMPLERTYPES id);
	void drawDebugWorld(const scene::CameraComponent & camera,CommandList cmd);
}