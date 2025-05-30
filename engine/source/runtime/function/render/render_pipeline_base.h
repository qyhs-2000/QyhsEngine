#pragma once
#include "render_resource_base.h"
#include "render_pass_base.h"
#include <memory>

#include "core/math/vector2.h"
#include "ui/editor_ui.h"

namespace qyhs
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
		void initializeUIRenderBackend(WindowUI* window_ui);
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr< RenderPassBase> m_main_camera_pass;
		std::shared_ptr<RenderPassBase>  m_pick_pass;
		std::shared_ptr<RenderPassBase>  m_combine_ui_pass;
		std::shared_ptr<RenderPassBase>  m_ui_pass;
	private:

	};
}
