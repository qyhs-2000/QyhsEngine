#pragma once
#include "function/render/rhi/rhi.h"
#include "render_resource_base.h"
#include "render_pass_base.h"
#include <memory>

#include "core/math/vector2.h"

namespace QYHS
{
	struct RenderPipelineInitInfo
	{
		std::shared_ptr<RenderResourceBase>m_render_resource;
	};

	class RenderPipelineBase
	{
	public:
		virtual void prepareDataForRenderPass(std::shared_ptr<RenderResourceBase> resource);
		virtual void initialize(RenderPipelineInitInfo init_info) = 0;
		virtual void render(std::shared_ptr<RenderResourceBase> render_resource) = 0;
		virtual uint32_t getGUIDOfPickedMesh(const Vector2 & picked_up) = 0;
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr< RenderPassBase> m_main_camera_pass;
		std::shared_ptr<RenderPassBase>  m_pick_pass;
	private:

	};
}
