#include "main_camera_render_pass.h"
#include "function/render/rhi/vulkan/vulkan_utils.h"
#include "core/utils/util.h"
#include <function/render/render_common.h>
#include <function/render/render_helper.h>
#include <cassert>
#include <iostream>
#include <stdexcept>
namespace QYHS
{

	void MainCameraRenderPass::draw()
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_framebuffer.render_pass;
		renderPassInfo.framebuffer = m_swapchain_framebuffers[m_vulkan_rhi->getCurrentFrameIndex()];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_vulkan_rhi->getSwapChainExtent();

		std::array<VkClearValue, 2> clearValues{};
		clearValues[0].color = { {0.0f, 0.0f, 0.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };

		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_vulkan_rhi->getCurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		drawMesh();

		vkCmdEndRenderPass(m_vulkan_rhi->getCurrentCommandBuffer());
	}

	void MainCameraRenderPass::drawMesh()
	{
		struct MeshNode
		{
			Matrix4x4 model_matrix;
		};
		//material map to the mesh that use it. and  render mesh nodes are the instance of the mesh
		std::map<VulkanMaterial*, std::map<VulkanMesh*, std::vector<MeshNode>>>	main_camera_pass_mesh_batch;
		
		uint32_t perframe_dynamic_offset = roundUp(m_global_render_resource->storage_buffer.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()], m_global_render_resource->storage_buffer.min_storage_buffer_offset_alignment);
		m_global_render_resource->storage_buffer.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] = perframe_dynamic_offset + sizeof(MeshPerFrameStorageBufferObject);
		assert(m_global_render_resource->storage_buffer.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] <= m_global_render_resource->storage_buffer.ringbuffer_begin[m_vulkan_rhi->getCurrentFrameIndex()] + m_global_render_resource->storage_buffer.ringbuffer_size[m_vulkan_rhi->getCurrentFrameIndex()]);
		*(reinterpret_cast<MeshPerFrameStorageBufferObject*>(reinterpret_cast<uintptr_t>(m_global_render_resource->storage_buffer.global_ringbuffer_memory_pointer) + perframe_dynamic_offset)) = m_mesh_perframe_storage_buffer_object;

		if (m_visible_render_meshes.p_main_camera_pass_visible_mesh_nodes)
		{
			for (RenderMeshNode& node : *(m_visible_render_meshes.p_main_camera_pass_visible_mesh_nodes))
			{
				auto& mesh_instance = main_camera_pass_mesh_batch[node.p_material];
				auto& mesh_nodes = mesh_instance[node.p_mesh];

				MeshNode tmp;
				tmp.model_matrix = node.model_matrix;

				mesh_nodes.push_back(tmp);
			}

			vkCmdBindPipeline(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[global_mesh].pipeline);

			VkViewport viewport{};
			viewport.x = 0.0f;
			viewport.y = 0.0f;
			viewport.width = (float)m_vulkan_rhi->getSwapChainExtent().width;
			viewport.height = (float)m_vulkan_rhi->getSwapChainExtent().height;
			viewport.minDepth = 0.0f;
			viewport.maxDepth = 1.0f;
			vkCmdSetViewport(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &viewport);

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = m_vulkan_rhi->getSwapChainExtent();
			vkCmdSetScissor(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &scissor);

			for (auto& material_map_to_meshinstance : main_camera_pass_mesh_batch)
			{
				VulkanMaterial& material = *(material_map_to_meshinstance.first);
				
				auto& mesh_instance = material_map_to_meshinstance.second;
				
				//material.material_descriptor_set = m_descriptors[per_mesh].descriptor_set;
				vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout, 1, 1, &material.material_descriptor_set, 0, nullptr);
				for (auto& pair : mesh_instance)
				{
					auto& mesh = *(pair.first);
					auto& mesh_nodes = pair.second;
					uint32_t total_instance_count = mesh_nodes.size();
					if (total_instance_count > 0)
					{
						uint32_t drawcall_max_instance_count =
							(sizeof(MeshPerdrawcallStorageBufferObject::mesh_instances) /
								sizeof(MeshPerdrawcallStorageBufferObject::mesh_instances[0]));
						uint32_t drawcall_count =
							roundUp(total_instance_count, drawcall_max_instance_count) / drawcall_max_instance_count;
						for (uint32_t drawcall_index = 0; drawcall_index < drawcall_count; ++drawcall_index)
						{
							uint32_t current_instance_count =
								((total_instance_count - drawcall_max_instance_count * drawcall_index) <
									drawcall_max_instance_count) ?
								(total_instance_count - drawcall_max_instance_count * drawcall_index) :
								drawcall_max_instance_count;

							// per drawcall storage buffer
							uint32_t perdrawcall_dynamic_offset =
								roundUp(m_global_render_resource->storage_buffer
									.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()],
									m_global_render_resource->storage_buffer.min_storage_buffer_offset_alignment);
							m_global_render_resource->storage_buffer
								.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] =
								perdrawcall_dynamic_offset + sizeof(MeshPerdrawcallStorageBufferObject);
							assert(m_global_render_resource->storage_buffer
								.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] <=
								(m_global_render_resource->storage_buffer
									.ringbuffer_begin[m_vulkan_rhi->getCurrentFrameIndex()] +
									m_global_render_resource->storage_buffer
									.ringbuffer_size[m_vulkan_rhi->getCurrentFrameIndex()]));
							MeshPerdrawcallStorageBufferObject& perdrawcall_storage_buffer_object =
								(*reinterpret_cast<MeshPerdrawcallStorageBufferObject*>(
									reinterpret_cast<uintptr_t>(m_global_render_resource->storage_buffer
										.global_ringbuffer_memory_pointer) +
									perdrawcall_dynamic_offset));

							for (int i = 0; i < current_instance_count; ++i)
							{
								perdrawcall_storage_buffer_object.mesh_instances[i].model_matrix =
									mesh_nodes[drawcall_max_instance_count * drawcall_index + i].model_matrix;
							}
							uint32_t dynamic_offsets[2] = { perframe_dynamic_offset,perdrawcall_dynamic_offset };
							vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(),
								VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout,
								0, 1, &m_descriptors[global_mesh].descriptor_set, 2, dynamic_offsets);
							VkBuffer vertexBuffers[] = { mesh.mesh_vertex_buffer };
							VkBuffer indices_buffer = mesh.mesh_vertex_index_buffer;
							size_t indices_count = mesh.indices_count;
							VkDeviceSize offsets[] = { 0 };

							vkCmdBindVertexBuffers(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, vertexBuffers, offsets);

							vkCmdBindIndexBuffer(m_vulkan_rhi->getCurrentCommandBuffer(), indices_buffer, 0, VK_INDEX_TYPE_UINT16);
							vkCmdDrawIndexed(m_vulkan_rhi->getCurrentCommandBuffer(), static_cast<uint32_t>(indices_count), current_instance_count, 0, 0, 0);
						}
					}	
				}
			}

		}
	}

	void MainCameraRenderPass::setupGlobalModelDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_vulkan_rhi->getDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &m_descriptors[global_mesh].descriptor_set_layout;

		if (vkAllocateDescriptorSets(m_vulkan_rhi->getDevice(), &allocInfo, &m_descriptors[global_mesh].descriptor_set) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate descriptor sets!");
		}

		for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			VkDescriptorBufferInfo bufferInfo{};
			bufferInfo.buffer = m_vulkan_rhi->getUniformBuffer(i);
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(UniformBufferObject);

			VkDescriptorBufferInfo mesh_perframe_storage_buffer = {};
			mesh_perframe_storage_buffer.offset = 0;
			mesh_perframe_storage_buffer.buffer = m_global_render_resource->storage_buffer.global_ringbuffer;
			mesh_perframe_storage_buffer.range = sizeof(MeshPerFrameStorageBufferObject);
			assert(mesh_perframe_storage_buffer.range < m_global_render_resource->storage_buffer.max_storage_buffer_size);

			VkDescriptorBufferInfo mesh_perdrawcall_storage_buffer = {};
			mesh_perdrawcall_storage_buffer.offset = 0;
			mesh_perdrawcall_storage_buffer.buffer = m_global_render_resource->storage_buffer.global_ringbuffer;
			mesh_perdrawcall_storage_buffer.range = sizeof(MeshPerdrawcallStorageBufferObject);
			


			VkWriteDescriptorSet descriptorWrites[3];

			descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[0].pNext = NULL;
			descriptorWrites[0].dstSet = m_descriptors[global_mesh].descriptor_set;
			descriptorWrites[0].dstBinding = 0;
			descriptorWrites[0].dstArrayElement = 0;
			descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrites[0].descriptorCount = 1;
			descriptorWrites[0].pBufferInfo = &bufferInfo;

			descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[1].pNext = NULL;
			descriptorWrites[1].dstSet = m_descriptors[global_mesh].descriptor_set;
			descriptorWrites[1].dstBinding = 1;
			descriptorWrites[1].dstArrayElement = 0;
			descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			descriptorWrites[1].descriptorCount = 1;
			descriptorWrites[1].pBufferInfo = &mesh_perframe_storage_buffer;

			descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrites[2].pNext = NULL;
			descriptorWrites[2].dstSet = m_descriptors[global_mesh].descriptor_set;
			descriptorWrites[2].dstBinding = 2;
			descriptorWrites[2].dstArrayElement = 0;
			descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			descriptorWrites[2].descriptorCount = 1;
			descriptorWrites[2].pBufferInfo = &mesh_perdrawcall_storage_buffer;
			
			vkUpdateDescriptorSets(m_vulkan_rhi->getDevice(), static_cast<uint32_t>(sizeof(descriptorWrites)/sizeof(descriptorWrites[0])), descriptorWrites, 0, nullptr);
		}
	}

	void MainCameraRenderPass::prepareData(std::shared_ptr<RenderResourceBase> resource)
	{
		RenderResource* m_resource = static_cast<RenderResource*>(resource.get());
		if (m_resource)
		{
			m_mesh_perframe_storage_buffer_object.project_view_matrix = m_resource->m_mesh_per_frame_storage_buffer_object.project_view_matrix;
		}
	}

	void MainCameraRenderPass::setupAttachments()
	{
		m_framebuffer.attachments.resize(main_camera_pass_attachment_count);
		VkExtent2D extent = m_vulkan_rhi->getSwapChainExtent();

		//set format of attachments
		m_framebuffer.attachments[main_camera_pass_color_attachment].format = m_vulkan_rhi->getSwapChainImageFormat();
		m_framebuffer.attachments[main_camera_pass_depth_attachment].format = m_vulkan_rhi->getDepthImageFormat();


		for (size_t i = 0; i < main_camera_pass_attachment_count; ++i)
		{
			//create color attachment
			VulkanUtils::createImage( m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice(), extent.width, extent.height,
				m_framebuffer.attachments[main_camera_pass_color_attachment].format,
				1, m_vulkan_rhi->getMSAASamples(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
				VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_framebuffer.attachments[main_camera_pass_color_attachment].image,
				m_framebuffer.attachments[main_camera_pass_color_attachment].memory);
			m_framebuffer.attachments[main_camera_pass_color_attachment].image_view
				= VulkanUtils::createImageView(m_vulkan_rhi->getDevice(),
					m_framebuffer.attachments[main_camera_pass_color_attachment].image,
					m_framebuffer.attachments[main_camera_pass_color_attachment].format,
					1, VK_IMAGE_ASPECT_COLOR_BIT);
			//create depth attachment
			VulkanUtils::createImage( m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice(),
				extent.width, extent.height, m_vulkan_rhi->getDepthImageFormat(), 1, m_vulkan_rhi->getMSAASamples(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
				m_framebuffer.attachments[main_camera_pass_depth_attachment].image, m_framebuffer.attachments[main_camera_pass_depth_attachment].memory);
			m_framebuffer.attachments[main_camera_pass_depth_attachment].image_view
				= VulkanUtils::createImageView(m_vulkan_rhi->getDevice(),
					m_framebuffer.attachments[main_camera_pass_depth_attachment].image,
					m_framebuffer.attachments[main_camera_pass_depth_attachment].format,
					1, VK_IMAGE_ASPECT_DEPTH_BIT);
			//create color attachment resolve   ,  this is for MSAA

		}
	}



	void MainCameraRenderPass::setupRenderPass()
	{
		VkAttachmentDescription attachments[3] = {};
		VkAttachmentDescription& colorAttachment = attachments[main_camera_pass_color_attachment];
		colorAttachment.format = m_vulkan_rhi->getSwapChainImageFormat();
		colorAttachment.samples = m_vulkan_rhi->getMSAASamples();
		colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachment.finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription& depthAttachment = attachments[main_camera_pass_depth_attachment];
		depthAttachment.format = m_vulkan_rhi->getDepthImageFormat();
		depthAttachment.samples = m_vulkan_rhi->getMSAASamples();
		depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentDescription& colorAttachmentResolve = attachments[main_camera_pass_color_for_msaa_attachment];
		colorAttachmentResolve.format = m_vulkan_rhi->getSwapChainImageFormat();
		colorAttachmentResolve.samples = VK_SAMPLE_COUNT_1_BIT;
		colorAttachmentResolve.loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		colorAttachmentResolve.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		colorAttachmentResolve.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		colorAttachmentResolve.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		colorAttachmentResolve.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference colorAttachmentRef{};
		colorAttachmentRef.attachment = &colorAttachment - attachments;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef{};
		depthAttachmentRef.attachment = &depthAttachment - attachments;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference colorAttachmentResolveRef{};
		colorAttachmentResolveRef.attachment = &colorAttachmentResolve - attachments;
		colorAttachmentResolveRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass[main_camera_subpass_count] = {};

		VkSubpassDescription& main_camera_base_subpass = subpass[main_camera_subpass_basepass];
		main_camera_base_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		main_camera_base_subpass.colorAttachmentCount = 1;
		main_camera_base_subpass.pColorAttachments = &colorAttachmentRef;
		main_camera_base_subpass.pDepthStencilAttachment = &depthAttachmentRef;
		main_camera_base_subpass.pResolveAttachments = &colorAttachmentResolveRef;

		VkSubpassDependency dependency[main_camera_subpass_dependency_count] = {};
		VkSubpassDependency& base_dependency = dependency[main_camera_subpass_base_dependency];
		base_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		base_dependency.dstSubpass = 0;
		base_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		base_dependency.srcAccessMask = 0;
		base_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		base_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = static_cast<uint32_t>(main_camera_pass_attachment_count);
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = main_camera_subpass_count;
		renderPassInfo.pSubpasses = subpass;
		renderPassInfo.dependencyCount = main_camera_subpass_dependency_count;
		renderPassInfo.pDependencies = dependency;

		if (vkCreateRenderPass(m_vulkan_rhi->getDevice(), &renderPassInfo, nullptr, &m_framebuffer.render_pass))
		{
			throw std::runtime_error("failed to create render pass");
		}
	}

	void MainCameraRenderPass::setupRenderPipelines()
	{
		m_render_pipelines.resize(render_pipeline_type_count);
		//mesh global buffer render pipeline
		{
			VkDescriptorSetLayout  descriptorSetLayout[2] = { m_descriptors[global_mesh].descriptor_set_layout,m_descriptors[mesh_per_material].descriptor_set_layout};
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			uint32_t descriptorset_layout_count = static_cast<uint32_t>(sizeof(descriptorSetLayout) / sizeof(descriptorSetLayout[0]));
			pipelineLayoutInfo.setLayoutCount = descriptorset_layout_count;
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

			if (vkCreatePipelineLayout(m_vulkan_rhi->getDevice(), &pipelineLayoutInfo, nullptr, &m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			auto vertShaderCode = Util::readFile("E://VS_Project//QyhsEngine//engine//shader//vert.spv");
			auto fragShaderCode = Util::readFile("E://VS_Project//QyhsEngine//engine//shader//frag.spv");

			VkShaderModule vertShaderModule = VulkanUtils::createShaderModule(m_vulkan_rhi->getDevice(), vertShaderCode);
			VkShaderModule fragShaderModule = VulkanUtils::createShaderModule(m_vulkan_rhi->getDevice(), fragShaderCode);

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

			auto bindingDescription = Vertex::getBindingDescription();
			auto attributeDescriptions = Vertex::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = 1;
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
			vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

			VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
			inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			inputAssembly.primitiveRestartEnable = VK_FALSE;

			VkPipelineViewportStateCreateInfo viewportState{};
			viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			viewportState.viewportCount = 1;
			viewportState.scissorCount = 1;

			VkPipelineRasterizationStateCreateInfo rasterizer{};
			rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			rasterizer.depthClampEnable = VK_FALSE;
			rasterizer.rasterizerDiscardEnable = VK_FALSE;
			rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
			rasterizer.lineWidth = 1.0f;
			rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
			rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
			rasterizer.depthBiasEnable = VK_FALSE;

			VkPipelineMultisampleStateCreateInfo multisampling{};
			multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisampling.sampleShadingEnable = VK_FALSE;
			multisampling.rasterizationSamples = m_vulkan_rhi->getMSAASamples();
			multisampling.sampleShadingEnable = VK_TRUE;
			multisampling.minSampleShading = .2f;

			VkPipelineDepthStencilStateCreateInfo depthStencil{};
			depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			depthStencil.depthTestEnable = VK_TRUE;
			depthStencil.depthWriteEnable = VK_TRUE;
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
			pipelineInfo.layout = m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout;
			pipelineInfo.renderPass = m_framebuffer.render_pass;
			pipelineInfo.subpass = 0;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

			if (vkCreateGraphicsPipelines(m_vulkan_rhi->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}

			vkDestroyShaderModule(m_vulkan_rhi->getDevice(), fragShaderModule, nullptr);
			vkDestroyShaderModule(m_vulkan_rhi->getDevice(), vertShaderModule, nullptr);
		}
	}

	void MainCameraRenderPass::setupDescriptorSets()
	{
		setupGlobalModelDescriptorSet();
	}

	void MainCameraRenderPass::setupDescriptorSetLayout()
	{
		m_descriptors.resize(descriptor_set_layout_type_count);
		{
			VkDescriptorSetLayoutBinding mesh_global_layout_bindings[3];
			//mesh descriptor set layout,like uniform which contain project matrix and view matrix;
			//set = 0,binding = 0
			VkDescriptorSetLayoutBinding& uboLayoutBinding = mesh_global_layout_bindings[0];
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.pImmutableSamplers = nullptr;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			//set = 0,binding = 1
			VkDescriptorSetLayoutBinding &mesh_global_layout_mesh_perframe_storage_buffer_binding = mesh_global_layout_bindings[1];
			mesh_global_layout_mesh_perframe_storage_buffer_binding.binding = 1;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.descriptorCount = 1;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.pImmutableSamplers = nullptr;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			//set = 0,binding = 2
			VkDescriptorSetLayoutBinding& mesh_global_layout_mesh_perdrawcall_storage_buffer_binding = mesh_global_layout_bindings[2];
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.binding = 2;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.descriptorCount = 1;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.pImmutableSamplers = nullptr;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = (sizeof(mesh_global_layout_bindings)/sizeof(mesh_global_layout_bindings[0]));
			layoutInfo.pBindings = mesh_global_layout_bindings;
			if (vkCreateDescriptorSetLayout(m_vulkan_rhi->getDevice(), &layoutInfo, nullptr, &m_descriptors[global_mesh].descriptor_set_layout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create descriptor set layout!");
			}
		}

		{
			//mesh per material descriptor set layout
			VkDescriptorSetLayoutBinding mesh_per_material_layout_binding[1];
			//set = 1,binding = 0
			VkDescriptorSetLayoutBinding& mesh_per_material_layout_base_color_sampler_binding = mesh_per_material_layout_binding[0];
			mesh_per_material_layout_base_color_sampler_binding.binding = 0;
			mesh_per_material_layout_base_color_sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			mesh_per_material_layout_base_color_sampler_binding.descriptorCount = 1;
			mesh_per_material_layout_base_color_sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			mesh_per_material_layout_base_color_sampler_binding.pImmutableSamplers = nullptr;
			
			VkDescriptorSetLayoutCreateInfo mesh_material_layout_create_info;
			mesh_material_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			mesh_material_layout_create_info.pNext = NULL;
			mesh_material_layout_create_info.flags = 0;
			mesh_material_layout_create_info.bindingCount = (sizeof(mesh_per_material_layout_binding)/sizeof(mesh_per_material_layout_binding[0]));
			mesh_material_layout_create_info.pBindings = mesh_per_material_layout_binding;

			if (vkCreateDescriptorSetLayout(m_vulkan_rhi->getDevice(),
				&mesh_material_layout_create_info,
				nullptr,
				&m_descriptors[mesh_per_material].descriptor_set_layout) != VK_SUCCESS)

			{
				throw std::runtime_error("create mesh material layout");
			}
		}

	}

	void setupFrameBuffer()
	{

	}

	void MainCameraRenderPass::initialize()
	{
		RenderPass::initialize();

		setupAttachments();
		setupRenderPass();
		setupFrameBuffer();
		setupDescriptorSetLayout();
		setupRenderPipelines();
		setupDescriptorSets();
		setupSwapChainFrameBuffers();
	}

	void MainCameraRenderPass::setupSwapChainFrameBuffers()
	{
		m_swapchain_framebuffers.resize(m_vulkan_rhi->getSwapChainImageView().size());

		for (size_t i = 0; i < m_vulkan_rhi->getSwapChainImageView().size(); i++) {
			std::array<VkImageView, 3> attachments = {
				m_framebuffer.attachments[main_camera_pass_color_attachment].image_view,
				m_framebuffer.attachments[main_camera_pass_depth_attachment].image_view,
				m_vulkan_rhi->getSwapChainImageView()[i]
			};

			VkFramebufferCreateInfo framebufferInfo{};
			framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			framebufferInfo.renderPass = m_framebuffer.render_pass;
			framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
			framebufferInfo.pAttachments = attachments.data();
			framebufferInfo.width = m_vulkan_rhi->getSwapChainExtent().width;
			framebufferInfo.height = m_vulkan_rhi->getSwapChainExtent().height;
			framebufferInfo.layers = 1;

			if (vkCreateFramebuffer(m_vulkan_rhi->getDevice(), &framebufferInfo, nullptr, &m_swapchain_framebuffers[i]) != VK_SUCCESS) {
				throw std::runtime_error("failed to create framebuffer!");
			}
		}

	}

}


