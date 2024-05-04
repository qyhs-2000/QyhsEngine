#pragma once
#include "function/render/rhi/rhi.h"
#include <memory>
namespace QYHS
{
	class RenderPipelineBase
	{
	public:
		virtual void initialize() = 0;
		virtual void render() = 0;
		std::shared_ptr<RHI> m_rhi;
		std::shared_ptr<class RenderPassBase> m_main_camera_pass;
	private:

	};
}