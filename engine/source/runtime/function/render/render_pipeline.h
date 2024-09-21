#pragma once
#include "render_pipeline_base.h"
#include <function/render/render_pass_base.h>


namespace QYHS
{
	class RenderPipeline :public RenderPipelineBase
	{
	public:
		virtual void initialize(RenderPipelineInitInfo init_info) override final;
		virtual void render(std::shared_ptr<RenderResourceBase> render_resource);
	private:
	};
}