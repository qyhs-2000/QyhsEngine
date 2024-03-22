#pragma once
#include "render_pipeline_base.h"
#include <function/render/render_pass_base.h>


namespace QYHS
{
	class RenderPipeline :public RenderPipelineBase
	{
	public:
		virtual void initialize() override final;
		virtual void render();
	private:
		std::shared_ptr<RenderPassBase>   m_main_camera_pass;
	};
}