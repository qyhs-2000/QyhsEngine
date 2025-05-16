#include "render_pass.h"
namespace qyhs
{
	RenderPass::VisibleRenderNode RenderPass::m_visible_render_meshes;
	void qyhs::RenderPass::initialize(RenderPassInitInfo * info)
	{
		m_vulkan_rhi = std::static_pointer_cast<VulkanRHI>(m_rhi);
		m_global_render_resource = &(std::static_pointer_cast<RenderResource>(m_render_resource)->m_global_render_resource);
	}

	const std::vector<VkImageView*> RenderPass::getFrameBufferImageViews()
	{
		std::vector<VkImageView *> image_views;
		for(int i = 0;i<m_framebuffer.attachments.size();++i)
		{
			image_views.push_back(&m_framebuffer.attachments[i].image_view);
		}
		return image_views;
	}
}
