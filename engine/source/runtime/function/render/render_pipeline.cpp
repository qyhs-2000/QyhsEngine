#include "render_pipeline.h"
#include <function/render/passes/main_camera_render_pass.h>

#include "passes/pick_render_pass.h"
#include "passes/combine_ui_pass.h"

void QYHS::RenderPipeline::initialize(RenderPipelineInitInfo init_info)
{

	m_main_camera_pass = std::make_shared<MainCameraRenderPass>();
	m_pick_pass = std::make_shared<PickRenderPass>();
	m_combine_ui_pass = std::make_shared<CombineUIPass>();
	m_ui_pass = std::make_shared<UIPass>();
	
	RenderPassCommonInfo pass_common_info;
	pass_common_info.rhi = m_rhi;
	pass_common_info.render_resource = init_info.m_render_resource;

	
	MainCameraPassInitInfo main_camera_init_info;
	m_main_camera_pass->setCommonInfo(pass_common_info);
	m_main_camera_pass->initialize(&main_camera_init_info);

	PickPassInitInfo pack_pass_init_info;
	m_pick_pass->setCommonInfo(pass_common_info);
	m_pick_pass->initialize(&pack_pass_init_info);

	MainCameraRenderPass* p_main_camera_pass = static_cast<MainCameraRenderPass*>(m_main_camera_pass.get());

	UIPassInitInfo ui_pass_init_info;
	ui_pass_init_info.render_pass = &(p_main_camera_pass->getRenderPass());
	m_ui_pass->setCommonInfo(pass_common_info);
	m_ui_pass->initialize(&ui_pass_init_info);

	CombineUIPassInitInfo combine_ui_pass_init_info;
	combine_ui_pass_init_info.scene_input_attachment = p_main_camera_pass->getFrameBufferImageViews()[main_camera_pass_backup_odd_color_attachment];
	combine_ui_pass_init_info.ui_input_attachment = p_main_camera_pass->getFrameBufferImageViews()[main_camera_pass_backup_even_color_attachment];
	combine_ui_pass_init_info.render_pass = static_cast<RenderPass*>(m_main_camera_pass.get())->getRenderPass();
	m_combine_ui_pass->setCommonInfo(pass_common_info);
	m_combine_ui_pass->initialize(&combine_ui_pass_init_info);
}

void QYHS::RenderPipeline::render(std::shared_ptr<RenderResourceBase> render_resource)
{

	VulkanRHI * vulkan_rhi = static_cast<VulkanRHI*>(m_rhi.get());
	RenderResource* m_render_resource = static_cast<RenderResource*>(render_resource.get());
	m_render_resource->resetRingBufferOffset(vulkan_rhi->getCurrentFrameIndex());
	vulkan_rhi->prepareBeforeRender(std::bind(&RenderPipeline::updatePassAfterRecreatePipeline,this));
	static_cast<MainCameraRenderPass*>(m_main_camera_pass.get())->draw(*static_cast<UIPass*>(m_ui_pass.get()),*static_cast<CombineUIPass*>(m_combine_ui_pass.get()));
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

void QYHS::RenderPipeline::setVisibleAxis(bool state)
{
	MainCameraRenderPass* main_camera_pass = static_cast<MainCameraRenderPass*>(m_main_camera_pass.get());
	main_camera_pass->setVisibleAxis(state);
}

void QYHS::RenderPipeline::setSelectedAxis(size_t selected_axis)
{
	static_cast<MainCameraRenderPass*>(m_main_camera_pass.get())->m_selected_axis = selected_axis;
}
