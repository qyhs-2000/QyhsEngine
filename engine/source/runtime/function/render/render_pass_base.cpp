#include "render_pass_base.h"

void QYHS::RenderPassBase::setCommonInfo(RenderPassCommonInfo common_info)
{
	m_rhi = common_info.rhi;
	m_render_resource = common_info.render_resource;
}