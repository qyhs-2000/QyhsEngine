#pragma once
#include "function/render/rhi/rhi.h"
#include <memory>
namespace QYHS
{
	struct RenderPassCommonInfo
	{

		std::shared_ptr<RHI>		rhi;
	};
	class RenderPassBase
	{
	public:
		virtual void initialize() = 0;
		virtual void setCommonInfo(RenderPassCommonInfo common_info);
	protected:
		std::shared_ptr<RHI>	m_rhi;
	};
}