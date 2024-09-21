#include "render_pass.h"
namespace QYHS
{
	RenderPass::VisibleRenderNode RenderPass::m_visible_render_meshes;
	void QYHS::RenderPass::initialize()
	{
		m_vulkan_rhi = std::static_pointer_cast<VulkanRHI>(m_rhi);
		m_global_render_resource = &(std::static_pointer_cast<RenderResource>(m_render_resource)->m_global_render_resource);
	}

}
