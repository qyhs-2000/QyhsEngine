#pragma once

#include "render_pass_base.h"
#include "function/render/rhi/vulkan/vulkan_rhi.h"
#include <function/render/render_pipeline_base.h>
#include <function/render/render_structure.h>
namespace QYHS
{

	enum
	{
		main_camera_subpass_basepass = 0,
		main_camera_subpass_count
	};

	enum
	{
		main_camera_subpass_base_dependency = 0,
		main_camera_subpass_dependency_count
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
	public:
		virtual void initialize() override;
	protected:
		std::shared_ptr<VulkanRHI>		m_vulkan_rhi{ nullptr };
		std::vector< Descriptor>		m_descriptors;
		std::vector<RenderPipelineBase>	m_render_pipelines;
		FrameBuffer						m_framebuffer;
	};
}