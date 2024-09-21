#pragma once
#include "function/render/rhi/rhi.h"
#include "render_resource_base.h"
#include <memory>
namespace QYHS
{
	struct RenderPipelineInitInfo
	{
		std::shared_ptr<RenderResourceBase>m_render_resource;
	};

	class RenderPipelineBase
	{
	public:
		virtual void initialize(RenderPipelineInitInfo init_info) = 0;
		virtual void render(std::shared_ptr<RenderResourceBase> render_resource) = 0;
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr<class RenderPassBase> m_main_camera_pass;
	private:

	};
}