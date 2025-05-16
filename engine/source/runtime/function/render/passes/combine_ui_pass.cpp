#include "combine_ui_pass.h"
#include "core/utils/utils.h"
namespace qyhs
{
	void CombineUIPass::initialize(RenderPassInitInfo * info)
	{
		RenderPass::initialize(info);
		const CombineUIPassInitInfo* init_info = static_cast<const CombineUIPassInitInfo*>(info);
		m_framebuffer.render_pass = *init_info->render_pass;
		m_scene_attachment = init_info->scene_input_attachment;
		m_ui_attachment = init_info->ui_input_attachment;

		setupDescriptorSetLayout();
		setupPipelines();
		setupDescriptorSet();
	}
	void CombineUIPass::setupDescriptorSetLayout()
	{
		m_descriptors.resize(1);

		VkDescriptorSetLayoutBinding layout_bindings[2];
		VkDescriptorSetLayoutBinding& scene_input_attachment_binding = layout_bindings[0];
		scene_input_attachment_binding.binding = 0;
		scene_input_attachment_binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		scene_input_attachment_binding.descriptor_count = 1;
		scene_input_attachment_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutBinding& ui_input_attachment_binding = layout_bindings[1];
		ui_input_attachment_binding.binding = 1;
		ui_input_attachment_binding.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		ui_input_attachment_binding.descriptor_count = 1;
		ui_input_attachment_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

		VkDescriptorSetLayoutCreateInfo layout_create_info{};
		layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layout_create_info.bindingCount = sizeof(layout_bindings) / sizeof(layout_bindings[0]);
		layout_create_info.pBindings = layout_bindings;
		layout_create_info.pNext = nullptr;

		m_vulkan_rhi->createDescriptorSetLayout(&layout_create_info, nullptr, m_descriptors[0].descriptor_set_layout);
	}

	void CombineUIPass::setupPipelines()
	{
		m_render_pipelines.resize(1);
		VkDescriptorSetLayout  descriptorSetLayout[1] = { *m_descriptors[0].descriptor_set_layout };
		VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		uint32_t descriptorset_layout_count = static_cast<uint32_t>(sizeof(descriptorSetLayout) / sizeof(descriptorSetLayout[0]));
		pipelineLayoutInfo.setLayoutCount = descriptorset_layout_count;
		pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

		if (vkCreatePipelineLayout(m_vulkan_rhi->getDevice(), &pipelineLayoutInfo, nullptr, &m_render_pipelines[0].pipeline_layout) != VK_SUCCESS) {
			throw std::runtime_error("failed to create pipeline layout!");
		}

		auto vertShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//combine_ui_vert.spv");
		auto fragShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//combine_ui_frag.spv");

		VkShaderModule vertShaderModule = m_vulkan_rhi->createShaderModule(m_vulkan_rhi->getDevice(), vertShaderCode);
		VkShaderModule fragShaderModule = m_vulkan_rhi->createShaderModule(m_vulkan_rhi->getDevice(), fragShaderCode);

		VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
		vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
		vertShaderStageInfo.module = vertShaderModule;
		vertShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
		fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		fragShaderStageInfo.module = fragShaderModule;
		fragShaderStageInfo.pName = "main";

		VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

		VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
		vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		vertexInputInfo.vertexBindingDescriptionCount = 0;
		vertexInputInfo.vertexAttributeDescriptionCount = 0;
		vertexInputInfo.pVertexBindingDescriptions = nullptr;
		vertexInputInfo.pVertexAttributeDescriptions = nullptr;

		VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
		inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
		inputAssembly.primitiveRestartEnable = VK_FALSE;

		VkPipelineViewportStateCreateInfo viewportState{};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = 1;
		viewportState.scissorCount = 1;
		viewportState.pViewports = &m_vulkan_rhi->getViewport();
		viewportState.pScissors = &m_vulkan_rhi->m_scissor;

		VkPipelineRasterizationStateCreateInfo rasterizer{};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = VK_FALSE;
		rasterizer.rasterizerDiscardEnable = VK_FALSE;
		rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
		rasterizer.lineWidth = 1.0f;
		rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
		rasterizer.depthBiasEnable = VK_FALSE;


		VkPipelineMultisampleStateCreateInfo multisampling{};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = VK_FALSE;
		multisampling.rasterizationSamples = m_vulkan_rhi->getMSAASamples();
		multisampling.sampleShadingEnable = VK_TRUE;
		multisampling.minSampleShading = .2f;

		VkPipelineDepthStencilStateCreateInfo depthStencil{};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = VK_FALSE;
		depthStencil.depthWriteEnable = VK_FALSE;
		depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
		depthStencil.depthBoundsTestEnable = VK_FALSE;
		depthStencil.stencilTestEnable = VK_FALSE;

		VkPipelineColorBlendAttachmentState colorBlendAttachment{};
		colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		colorBlendAttachment.blendEnable = VK_FALSE;

		VkPipelineColorBlendStateCreateInfo colorBlending{};
		colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colorBlending.logicOpEnable = VK_FALSE;
		colorBlending.logicOp = VK_LOGIC_OP_COPY;
		colorBlending.attachmentCount = 1;
		colorBlending.pAttachments = &colorBlendAttachment;
		colorBlending.blendConstants[0] = 0.0f;
		colorBlending.blendConstants[1] = 0.0f;
		colorBlending.blendConstants[2] = 0.0f;
		colorBlending.blendConstants[3] = 0.0f;

		std::vector<VkDynamicState> dynamicStates = {
			VK_DYNAMIC_STATE_VIEWPORT,
			VK_DYNAMIC_STATE_SCISSOR
		};
		VkPipelineDynamicStateCreateInfo dynamicState{};
		dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
		dynamicState.pDynamicStates = dynamicStates.data();



		VkGraphicsPipelineCreateInfo pipelineInfo{};
		pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipelineInfo.stageCount = 2;
		pipelineInfo.pStages = shaderStages;
		pipelineInfo.pVertexInputState = &vertexInputInfo;
		pipelineInfo.pInputAssemblyState = &inputAssembly;
		pipelineInfo.pViewportState = &viewportState;
		pipelineInfo.pRasterizationState = &rasterizer;
		pipelineInfo.pMultisampleState = &multisampling;
		pipelineInfo.pDepthStencilState = &depthStencil;
		pipelineInfo.pColorBlendState = &colorBlending;
		pipelineInfo.pDynamicState = &dynamicState;
		pipelineInfo.layout = m_render_pipelines[0].pipeline_layout;
		pipelineInfo.renderPass = m_framebuffer.render_pass;
		pipelineInfo.subpass = main_camera_subpass_combine_ui;
		pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

		if (vkCreateGraphicsPipelines(m_vulkan_rhi->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_render_pipelines[0].pipeline) != VK_SUCCESS) {
			throw std::runtime_error("failed to create graphics pipeline!");
		}

		vkDestroyShaderModule(m_vulkan_rhi->getDevice(), fragShaderModule, nullptr);
		vkDestroyShaderModule(m_vulkan_rhi->getDevice(), vertShaderModule, nullptr);
	}

	void CombineUIPass::setupDescriptorSet()
	{
		m_vulkan_rhi->allocateDescriptorSets(m_descriptors[0].descriptor_set_layout, 1, m_descriptors[0].descriptor_set);

		VkDescriptorImageInfo per_frame_scene_input_attachment_info = {};
		per_frame_scene_input_attachment_info.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(),m_vulkan_rhi->getDevice());
		per_frame_scene_input_attachment_info.imageView   = *m_scene_attachment;
		per_frame_scene_input_attachment_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo per_frame_ui_input_attachment_info = {};
		per_frame_ui_input_attachment_info.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(),m_vulkan_rhi->getDevice());
		per_frame_ui_input_attachment_info.imageView   = *m_ui_attachment;
		per_frame_ui_input_attachment_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::array<VkWriteDescriptorSet,2> post_process_descriptor_writes_info;

		VkWriteDescriptorSet& per_frame_scene_input_attachment_write_info = post_process_descriptor_writes_info[0];
		per_frame_scene_input_attachment_write_info.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		per_frame_scene_input_attachment_write_info.pNext                 = NULL;
		per_frame_scene_input_attachment_write_info.dstSet                = *m_descriptors[0].descriptor_set;
		per_frame_scene_input_attachment_write_info.dstBinding            = 0;
		per_frame_scene_input_attachment_write_info.dstArrayElement       = 0;
		per_frame_scene_input_attachment_write_info.descriptorType        = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		per_frame_scene_input_attachment_write_info.descriptorCount       = 1;
		per_frame_scene_input_attachment_write_info.pImageInfo            = &per_frame_scene_input_attachment_info;

		VkWriteDescriptorSet& per_frame_ui_input_attachment_write_info = post_process_descriptor_writes_info[1];
		per_frame_ui_input_attachment_write_info.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		per_frame_ui_input_attachment_write_info.pNext                 = NULL;
		per_frame_ui_input_attachment_write_info.dstSet                = *m_descriptors[0].descriptor_set;
		per_frame_ui_input_attachment_write_info.dstBinding            = 1;
		per_frame_ui_input_attachment_write_info.dstArrayElement       = 0;
		per_frame_ui_input_attachment_write_info.descriptorType        = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		per_frame_ui_input_attachment_write_info.descriptorCount       = 1;
		per_frame_ui_input_attachment_write_info.pImageInfo            = &per_frame_ui_input_attachment_info;

		vkUpdateDescriptorSets(m_vulkan_rhi->m_device,post_process_descriptor_writes_info.size(),
			post_process_descriptor_writes_info.data(),
			0,
			NULL);
	}

	void CombineUIPass::draw()
	{
		VkViewport viewport = {0.0,
			0.0,
			static_cast<float>(m_vulkan_rhi->getSwapChainExtent().width),
			static_cast<float>(m_vulkan_rhi->getSwapChainExtent().height),
			0.0,
			1.0};
		VkRect2D   scissor  = {0, 0, m_vulkan_rhi->getSwapChainExtent().width, m_vulkan_rhi->getSwapChainExtent().height};
		vkCmdBindPipeline(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[0].pipeline);
		vkCmdSetViewport(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &viewport);
		vkCmdSetScissor(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &scissor);
		
		vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_render_pipelines[0].pipeline_layout, 0, 1,
			m_descriptors[0].descriptor_set, 0, nullptr);

		vkCmdDraw(m_vulkan_rhi->getCurrentCommandBuffer(), 3, 1, 0, 0);
	}

	void CombineUIPass::updateAfterRecreateSwapChain(VkImageView* input_color_attachment, VkImageView* input_ui_attachment)
	{
		VkDescriptorImageInfo per_frame_scene_input_attachment_info = {};
		per_frame_scene_input_attachment_info.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(),m_vulkan_rhi->getDevice());
		per_frame_scene_input_attachment_info.imageView   = *input_color_attachment;
		per_frame_scene_input_attachment_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkDescriptorImageInfo per_frame_ui_input_attachment_info = {};
		per_frame_ui_input_attachment_info.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(),m_vulkan_rhi->getDevice());
		per_frame_ui_input_attachment_info.imageView   = *input_ui_attachment;
		per_frame_ui_input_attachment_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		std::array<VkWriteDescriptorSet,2> post_process_descriptor_writes_info;

		VkWriteDescriptorSet& per_frame_scene_input_attachment_write_info = post_process_descriptor_writes_info[0];
		per_frame_scene_input_attachment_write_info.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		per_frame_scene_input_attachment_write_info.pNext                 = NULL;
		per_frame_scene_input_attachment_write_info.dstSet                = *m_descriptors[0].descriptor_set;
		per_frame_scene_input_attachment_write_info.dstBinding            = 0;
		per_frame_scene_input_attachment_write_info.dstArrayElement       = 0;
		per_frame_scene_input_attachment_write_info.descriptorType        = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		per_frame_scene_input_attachment_write_info.descriptorCount       = 1;
		per_frame_scene_input_attachment_write_info.pImageInfo            = &per_frame_scene_input_attachment_info;

		VkWriteDescriptorSet& per_frame_ui_input_attachment_write_info = post_process_descriptor_writes_info[1];
		per_frame_ui_input_attachment_write_info.sType                 = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		per_frame_ui_input_attachment_write_info.pNext                 = NULL;
		per_frame_ui_input_attachment_write_info.dstSet                = *m_descriptors[0].descriptor_set;
		per_frame_ui_input_attachment_write_info.dstBinding            = 1;
		per_frame_ui_input_attachment_write_info.dstArrayElement       = 0;
		per_frame_ui_input_attachment_write_info.descriptorType        = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		per_frame_ui_input_attachment_write_info.descriptorCount       = 1;
		per_frame_ui_input_attachment_write_info.pImageInfo            = &per_frame_ui_input_attachment_info;

		vkUpdateDescriptorSets(m_vulkan_rhi->m_device,post_process_descriptor_writes_info.size(),
			post_process_descriptor_writes_info.data(),
			0,
			NULL);
		
	}
}