#pragma once
#include "function/render/rhi/vulkan/vulkan_rhi.h"
namespace QYHS
{
	class RenderPipelineBase
	{
	public:
		virtual void initialize() = 0;
		virtual void render() = 0;
		std::shared_ptr<RHI> m_rhi;

	private:

	};
}