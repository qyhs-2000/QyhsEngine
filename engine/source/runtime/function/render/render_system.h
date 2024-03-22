#pragma once
#include "function/render/render_pipeline.h"
#include <function/render/render_swap_context.h>
#include <function/render/render_scene.h>
#include <function/render/render_guid_allocator.h>
namespace QYHS
{
	

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
		RenderSwapContext& getSwapContext() { return m_swap_context; }
	private:
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr< RenderPipelineBase>	m_render_pipeline;
		RenderSwapContext						m_swap_context;
		std::shared_ptr<RenderScene>			m_render_scene;
		std::shared_ptr<RenderResource>			m_render_resource;
		
	};

	
}