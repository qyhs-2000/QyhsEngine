#pragma once
#include "function/render/render_pipeline.h"
#include <function/render/render_swap_context.h>
#include <function/render/render_scene.h>

#include "render_camera.h"
namespace QYHS
{
	
	class RenderCamera;
	class RenderSystem
	{
	public:
		RenderSystem() = default;
		~RenderSystem() {};
		void tick();
		void initialize();
		void swapLogicRenderData();
		void processRenderEntity();
		void processSwapData();
		size_t getGObjectIDByMeshID(size_t mesh_id);
		RenderSwapContext& getSwapContext() { return m_swap_context; }
		std::shared_ptr<RenderCamera> getRenderCamera() { return m_render_camera; }
		uint32_t getMeshIDByPickedUV(Vector2 picked_uv);
		void setVisibleAxis(std::optional<RenderEntity> axis);
		RenderGUIDAllocator<MeshSourceDesc> & getMeshAssetIDAllocator();
		RenderGUIDAllocator<GameObjectPartId> & getInstanceIDAllocator();
		void uploadGameResource(RenderEntity * entity,RenderMeshData mesh_data);
	private:
		std::shared_ptr<RenderCamera>			m_render_camera;
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr< RenderPipelineBase>	m_render_pipeline;
		RenderSwapContext						m_swap_context;
		std::shared_ptr<RenderScene>			m_render_scene;
		std::shared_ptr<RenderResource>			m_render_resource;
		
	};

	
}