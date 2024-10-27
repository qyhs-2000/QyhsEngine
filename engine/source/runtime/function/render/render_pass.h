#pragma once

#include "render_pass_base.h"
#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include <function/render/render_pipeline_base.h>
#include <function/render/render_structure.h>
#include "render_resource.h"
namespace QYHS
{

	enum
	{
		main_camera_subpass_basepass = 0,
		main_camera_subpass_skybox = 1,
		main_camera_subpass_ui = 2,
		main_camera_subpass_combine_ui = 3,
		main_camera_subpass_count
	};

	enum
	{
		main_camera_subpass_base_dependency = 0,
		main_camera_subpass_skybox_dependency,
		main_camera_subpass_dependency_ui,
		main_camera_subpass_dependency_combine_ui,
		main_camera_subpass_dependency_count
	};

	enum MainCameraPassAttachment
	{
		main_camera_pass_base_color_attachment = 0,
		main_camera_pass_depth_attachment ,
		main_camera_pass_backup_odd_color_attachment,  //缓存attachment_1
		main_camera_pass_backup_even_color_attachment, //缓存attachment_2
		main_camera_pass_swap_chain_image_attachment ,
		main_camera_pass_attachment_count 
	};

	struct Descriptor
	{
		VkDescriptorSet descriptor_set;
		VkDescriptorSetLayout descriptor_set_layout;
	};

	class RenderPass :public RenderPassBase
	{
		friend class RenderSystem;
	public:
		struct FrameBufferAttachment
		{
			VkImage image;
			VkImageView image_view;
			VkFormat format;
			VkDeviceMemory memory;
		};

		struct VisibleRenderNode
		{
			std::vector<RenderMeshNode>* p_main_camera_pass_visible_mesh_nodes{ nullptr };
			RenderMeshNode* p_axis_node;
		};

		struct FrameBuffer
		{
			VkRenderPass render_pass;
			VkFramebuffer framebuffer;
			std::vector<FrameBufferAttachment> attachments;
		};

		struct RenderPipelineBase
		{
			VkPipeline			pipeline;
			VkPipelineLayout	pipeline_layout;
		};
		static VisibleRenderNode m_visible_render_meshes;
		GlobalRenderResource* m_global_render_resource{ nullptr };
	public:
		virtual void initialize(RenderPassInitInfo * info) override;
		VkRenderPass* getRenderPass() { return &m_framebuffer.render_pass; };
		const std::vector<VkImageView*>  getFrameBufferImageViews();
	protected:
		std::shared_ptr<VulkanRHI>		m_vulkan_rhi{ nullptr };
		std::vector< Descriptor>		m_descriptors;
		std::vector<RenderPipelineBase>	m_render_pipelines;
		FrameBuffer						m_framebuffer;

		
	};
}