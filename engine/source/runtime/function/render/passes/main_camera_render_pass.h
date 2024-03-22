#pragma once
#include "function/render/render_pass.h"

namespace QYHS
{
	/*
	* @param 1. model
	*
	*/
	enum RenderPipelineType
	{
		render_pipeline_type_mesh_buffer = 0,
		render_pipeline_type_count
	};

	enum DescriptorSetLayoutType
	{
		per_mesh=0,
		descriptor_set_layout_type_count
	};

	enum MainCameraPassAttachment
	{
		main_camera_pass_color_attachment = 0,
		main_camera_pass_depth_attachment = 1,
		main_camera_pass_color_for_msaa_attachment = 2,
		main_camera_pass_attachment_count = 3
	};

	class MainCameraRenderPass :public RenderPass
	{
	public:
		void draw();
		void setupRenderPass();
		void setupAttachments();
		void setupRenderPipelines();
		void setupDescriptorSetLayout();
		void setupDescriptorSets();
		virtual void initialize() override final;
		void setupSwapChainFrameBuffers();
		void drawMesh();
	private:
		std::vector<VkFramebuffer> m_swapchain_framebuffers;
		
	};
}