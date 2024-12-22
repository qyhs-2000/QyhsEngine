#pragma once
#include "function/render/render_pipeline.h"
#include <function/render/render_swap_context.h>
#include <function/render/render_scene.h>

#include "render_camera.h"
#include "ui/editor_ui.h"

namespace QYHS
{
	
	class RenderCamera;
	class MeshComponentRes;
	class MaterialRes;
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() {};
		void tick();
		void initialize();
		void swapLogicRenderData();
		void processSwapData();
		void changeRenderCameraType(RenderCameraType camera_type) { m_render_camera->changeType(camera_type); }
		void postLoadMeshComponentRes(MeshComponentRes& mesh_res);
		size_t getGObjectIDByMeshID(size_t mesh_id);
		RenderSwapContext& getSwapContext() { return m_swap_context; }
		std::shared_ptr<RenderCamera> getRenderCamera() { return m_render_camera; }
		std::shared_ptr<RenderScene>  getRenderScene() { return m_render_scene; }
		uint32_t getMeshIDByPickedUV(Vector2 picked_uv);
		void setVisibleAxis(std::optional<RenderEntity> axis);
		RenderGUIDAllocator<MeshSourceDesc> & getMeshAssetIDAllocator();
		RenderGUIDAllocator<GameObjectPartId> & getInstanceIDAllocator();
		void updateEngineContentViewport(Vector2 content_position, Vector2 content_size);
		void uploadGameResource(RenderEntity * entity,RenderMeshData mesh_data);
		void setSelectedAxis(size_t selected_axis);
		void initializeUIRenderBackend(WindowUI* ui);
		size_t createMaterial(RenderMaterialData& material_data);
		size_t postAndLoadMeshSource(const std::string& file_path);
		size_t postAndLoadMaterialSource(MaterialRes & material_res,bool with_texture);
		size_t createNewEntity(std::string name);
		size_t createMesh(RenderMeshData& mesh_data);
		std::shared_ptr<RHI> getRHI() { return m_rhi; }
	private:
		std::shared_ptr<RenderCamera>			m_render_camera;
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr< RenderPipelineBase>	m_render_pipeline;
		RenderSwapContext						m_swap_context;
		std::shared_ptr<RenderScene>			m_render_scene;
		std::shared_ptr<RenderResource>			m_render_resource;
	};
}