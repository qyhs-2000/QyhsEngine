#include "render_pipeline.h"
#include <function/render/passes/main_camera_render_pass.h>

#include "passes/pick_render_pass.h"

void QYHS::RenderPipeline::initialize(RenderPipelineInitInfo init_info)
{

	m_main_camera_pass = std::make_shared<MainCameraRenderPass>();
	m_pick_pass = std::make_shared<PickRenderPass>();

	RenderPassCommonInfo pass_common_info;
	pass_common_info.rhi = m_rhi;
	pass_common_info.render_resource = init_info.m_render_resource;

	m_main_camera_pass->setCommonInfo(pass_common_info);
	m_main_camera_pass->initialize();

	m_pick_pass->setCommonInfo(pass_common_info);
	m_pick_pass->initialize();
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

uint32_t QYHS::RenderPipeline::getGUIDOfPickedMesh(const Vector2& picked_uv)
{
	PickRenderPass * pick_pass = static_cast<PickRenderPass*>(m_pick_pass.get());
	return pick_pass->pick(picked_uv);
}

void QYHS::RenderPipeline::updatePassAfterRecreatePipeline()
{
	MainCameraRenderPass* main_camera_pass = static_cast<MainCameraRenderPass*>(m_main_camera_pass.get());
	main_camera_pass->updateAfterRecreateSwapChain();
}
