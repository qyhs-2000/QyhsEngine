#include "render_pipeline.h"
#include <function/render/passes/main_camera_render_pass.h>

void QYHS::RenderPipeline::initialize(RenderPipelineInitInfo init_info)
{

	m_main_camera_pass = std::make_shared<MainCameraRenderPass>();

	RenderPassCommonInfo pass_common_info;
	pass_common_info.rhi = m_rhi;
	pass_common_info.render_resource = init_info.m_render_resource;

	m_main_camera_pass->setCommonInfo(pass_common_info);
	m_main_camera_pass->initialize();
}

void QYHS::RenderPipeline::render(std::shared_ptr<RenderResourceBase> render_resource)
{

	VulkanRHI * vulkan_rhi = static_cast<VulkanRHI*>(m_rhi.get());
	RenderResource* m_render_resource = static_cast<RenderResource*>(render_resource.get());
	m_render_resource->resetRingBufferOffset(vulkan_rhi->getCurrentFrameIndex());
	vulkan_rhi->prepareBeforeRender(std::bind(&RenderPipeline::updatePassAfterRecreatePipeline,this));
	static_cast<MainCameraRenderPass*>(m_main_camera_pass.get())->draw();
	vulkan_rhi->submitRender(std::bind(&RenderPipeline::updatePassAfterRecreatePipeline,this));

	
}

void QYHS::RenderPipeline::updatePassAfterRecreatePipeline()
{
	MainCameraRenderPass* main_camera_pass = static_cast<MainCameraRenderPass*>(m_main_camera_pass.get());
	main_camera_pass->updateAfterRecreateSwapChain();
}
