#pragma once
#include "function/render/rhi/rhi.h"
#include <memory>
#include "render_resource_base.h"
namespace QYHS
{
	

	struct RenderPassCommonInfo
	{

		std::shared_ptr<RHI>		rhi;
		std::shared_ptr<RenderResourceBase> render_resource;
	};
	class RenderPassBase
	{
	public:
		virtual void initialize() = 0;
		virtual void setCommonInfo(RenderPassCommonInfo common_info);
	protected:
		std::shared_ptr<RHI>	m_rhi;
		std::shared_ptr<RenderResourceBase> m_render_resource;
	};
}