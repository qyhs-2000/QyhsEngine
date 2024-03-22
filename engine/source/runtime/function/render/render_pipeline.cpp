#include "render_pipeline.h"
#include <function/render/passes/main_camera_render_pass.h>

void QYHS::RenderPipeline::initialize()
{

	m_main_camera_pass = std::make_shared<MainCameraRenderPass>();

	RenderPassCommonInfo pass_common_info;
	pass_common_info.rhi = m_rhi;

	m_main_camera_pass->setCommonInfo(pass_common_info);
	m_main_camera_pass->initialize();
}

void QYHS::RenderPipeline::render()
{

	VulkanRHI * vulkan_rhi = static_cast<VulkanRHI*>(m_rhi.get());
	/*vulkan_rhi->waitForFence();
	vulkan_rhi->getNextImage();
	vulkan_rhi->updateUniformBuffer();
	vulkan_rhi->resetFence();
	vulkan_rhi->resetCommandBuffer();
	vulkan_rhi->recordCommandBuffer();*/
	vulkan_rhi->prepareBeforeRender();
	static_cast<MainCameraRenderPass*>(m_main_camera_pass.get())->draw();
	vulkan_rhi->submitCommandBuffer();

	
}
