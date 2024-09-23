#pragma once
#include "function/render/render_pass.h"
#include "function/render/render_common.h"
namespace QYHS
{
	/*
	* @param 1. model
	*
	*/

	class MainCameraRenderPass :public RenderPass
	{
	public:
	enum RenderPipelineType
	{
		render_pipeline_type_mesh_global_buffer = 0,
		render_pipeline_type_count
	};

	enum DescriptorSetLayoutType
	{
		global_mesh=0,
		mesh_per_material,
		descriptor_set_layout_type_count
	};

	enum MainCameraPassAttachment
	{
		main_camera_pass_color_attachment = 0,
		main_camera_pass_depth_attachment = 1,
		main_camera_pass_color_for_msaa_attachment = 2,
		main_camera_pass_attachment_count = 3
	};
		void draw();
		void setupRenderPass();
		void setupAttachments();
		void setupRenderPipelines();
		void setupDescriptorSetLayout();
		void setupDescriptorSets();
		virtual void initialize() override final;
		void setupSwapChainFrameBuffers();
		void drawMesh();
		void setupGlobalModelDescriptorSet();
		virtual void prepareData(std::shared_ptr<RenderResourceBase> resource) override;
		MeshPerFrameStorageBufferObject m_mesh_perframe_storage_buffer_object;
	private:
		std::vector<VkFramebuffer> m_swapchain_framebuffers;
		
	};
}