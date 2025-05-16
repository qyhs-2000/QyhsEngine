#pragma once
#include "render_pipeline_base.h"
#include <function/render/render_pass_base.h>

namespace qyhs
{
	class RenderPipeline :public RenderPipelineBase
	{
	public:
		virtual void initialize(RenderPipelineInitInfo init_info) override final;
		virtual void render(std::shared_ptr<RenderResourceBase> render_resource);
		virtual uint32_t getGUIDOfPickedMesh(const Vector2 & picked_uv) override final;
		void updatePassAfterRecreatePipeline();
		void setVisibleAxis(bool state);
		void setSelectedAxis(size_t selected_axis);
	private:
	};
}
