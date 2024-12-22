#include "main_camera_render_pass.h"
#include "core/utils/utils.h"
#include <function/render/render_common.h>
#include <function/render/render_helper.h>
#include <cassert>
#include <array>
#include <stdexcept>
namespace QYHS
{
	void MainCameraRenderPass::initialize(RenderPassInitInfo* info)
	{
		RenderPass::initialize(info);
		//setupSkyboxCubeBuffer();
		setupAttachments();
		setupRenderPass();
		setupDescriptorSetLayout();
		setupRenderPipelines();
		setupDescriptorSets();
		//setupFrameBuffer();
		setupSwapChainFrameBuffers();
	}

	void MainCameraRenderPass::setupAttachments()
	{
		m_framebuffer.attachments.resize(main_camera_pass_attachment_count);
		VkExtent2D extent = m_vulkan_rhi->getSwapChainExtent();

		//set format of attachments
		m_framebuffer.attachments[main_camera_pass_base_color_attachment].format = m_vulkan_rhi->getSwapChainImageFormat();
		m_framebuffer.attachments[main_camera_pass_depth_attachment].format = m_vulkan_rhi->getDepthImageFormat();
		m_framebuffer.attachments[main_camera_pass_backup_odd_color_attachment].format = m_vulkan_rhi->getSwapChainImageFormat();
		m_framebuffer.attachments[main_camera_pass_backup_even_color_attachment].format = m_vulkan_rhi->getSwapChainImageFormat();
		//m_framebuffer.attachments[main_camera_pass_skybox_attachment].format = m_vulkan_rhi->getSwapChainImageFormat();

		for (size_t i = 0; i < main_camera_pass_attachment_count; ++i)
		{
			if (i == main_camera_pass_base_color_attachment || i == main_camera_pass_backup_odd_color_attachment || i == main_camera_pass_backup_even_color_attachment)
			{
				m_vulkan_rhi->createImage(m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice(), extent.width, extent.height,
					m_framebuffer.attachments[i].format,
					1, m_vulkan_rhi->getMSAASamples(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
					VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_framebuffer.attachments[i].image,
					m_framebuffer.attachments[i].memory);
				m_framebuffer.attachments[i].image_view
					= m_vulkan_rhi->createImageView(m_vulkan_rhi->getDevice(),
						m_framebuffer.attachments[i].image,
						m_framebuffer.attachments[i].format, VK_IMAGE_VIEW_TYPE_2D, 1,
						1, VK_IMAGE_ASPECT_COLOR_BIT);

			}
			else if (i == main_camera_pass_depth_attachment)
			{
				//create depth attachment
				m_vulkan_rhi->createImage(m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice(),
					extent.width, extent.height, m_vulkan_rhi->getDepthImageFormat(), 1, m_vulkan_rhi->getMSAASamples(), VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
					m_framebuffer.attachments[main_camera_pass_depth_attachment].image, m_framebuffer.attachments[main_camera_pass_depth_attachment].memory);
				m_framebuffer.attachments[main_camera_pass_depth_attachment].image_view
					= m_vulkan_rhi->createImageView(m_vulkan_rhi->getDevice(),
						m_framebuffer.attachments[main_camera_pass_depth_attachment].image,
						m_framebuffer.attachments[main_camera_pass_depth_attachment].format, VK_IMAGE_VIEW_TYPE_2D, 1,
						1, VK_IMAGE_ASPECT_DEPTH_BIT);

			}

		}
	}

	void MainCameraRenderPass::setupGlobalModelDescriptorSet()
	{
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = m_vulkan_rhi->getDescriptorPool();
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = m_descriptors[global_mesh].descriptor_set_layout;

		m_vulkan_rhi->allocateDescriptorSets(m_descriptors[global_mesh].descriptor_set_layout, 1, m_descriptors[global_mesh].descriptor_set);


		VkDescriptorBufferInfo mesh_perframe_storage_buffer = {};
		mesh_perframe_storage_buffer.offset = 0;
		mesh_perframe_storage_buffer.buffer = m_global_render_resource->storage_buffer.global_ringbuffer;
		mesh_perframe_storage_buffer.range = sizeof(MeshPerFrameStorageBufferObject);
		assert(mesh_perframe_storage_buffer.range < m_global_render_resource->storage_buffer.max_storage_buffer_size);

		VkDescriptorBufferInfo mesh_perdrawcall_storage_buffer = {};
		mesh_perdrawcall_storage_buffer.offset = 0;
		mesh_perdrawcall_storage_buffer.buffer = m_global_render_resource->storage_buffer.global_ringbuffer;
		mesh_perdrawcall_storage_buffer.range = sizeof(MeshPerdrawcallStorageBufferObject);

		VkDescriptorBufferInfo mesh_perdrawcall_vertex_blending_buffer = {};
		mesh_perdrawcall_vertex_blending_buffer.offset = 0;
		mesh_perdrawcall_vertex_blending_buffer.buffer = m_global_render_resource->storage_buffer.global_ringbuffer;
		mesh_perdrawcall_vertex_blending_buffer.range = sizeof(MeshPerdrawcallVertexBlendingStorageBufferObject);

		VkWriteDescriptorSet descriptorWrites[3];

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].pNext = NULL;
		descriptorWrites[0].dstSet = *m_descriptors[global_mesh].descriptor_set;
		descriptorWrites[0].dstBinding = 1;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &mesh_perframe_storage_buffer;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].pNext = NULL;
		descriptorWrites[1].dstSet = *m_descriptors[global_mesh].descriptor_set;
		descriptorWrites[1].dstBinding = 2;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &mesh_perdrawcall_storage_buffer;

		descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[2].pNext = NULL;
		descriptorWrites[2].dstSet = *m_descriptors[global_mesh].descriptor_set;
		descriptorWrites[2].dstBinding = 3;
		descriptorWrites[2].dstArrayElement = 0;
		descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		descriptorWrites[2].descriptorCount = 1;
		descriptorWrites[2].pBufferInfo = &mesh_perdrawcall_vertex_blending_buffer;

		vkUpdateDescriptorSets(m_vulkan_rhi->getDevice(), static_cast<uint32_t>(sizeof(descriptorWrites) / sizeof(descriptorWrites[0])), descriptorWrites, 0, nullptr);
	}

	void MainCameraRenderPass::setAxisDescriptorSet()
	{
		m_vulkan_rhi->allocateDescriptorSets(m_descriptors[axis].descriptor_set_layout, 1, m_descriptors[axis].descriptor_set);

		VkDescriptorBufferInfo mesh_perframe_storage_buffer = {};
		mesh_perframe_storage_buffer.offset = 0;
		mesh_perframe_storage_buffer.buffer = m_global_render_resource->storage_buffer.global_ringbuffer;
		mesh_perframe_storage_buffer.range = sizeof(MeshPerFrameStorageBufferObject);
		assert(mesh_perframe_storage_buffer.range < m_global_render_resource->storage_buffer.max_storage_buffer_size);

		VkDescriptorBufferInfo axis_storage_buffer = {};
		axis_storage_buffer.offset = 0;
		axis_storage_buffer.buffer = m_global_render_resource->storage_buffer.axis_storage_buffer;
		axis_storage_buffer.range = sizeof(AxisStorageBufferObject);

		VkDescriptorImageInfo backup_color_attachment = {};
		backup_color_attachment.imageView = m_framebuffer.attachments[main_camera_pass_backup_odd_color_attachment].image_view;
		backup_color_attachment.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice());
		backup_color_attachment.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet descriptorWrites[2];

		descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[0].pNext = NULL;
		descriptorWrites[0].dstSet = *m_descriptors[axis].descriptor_set;
		descriptorWrites[0].dstBinding = 0;
		descriptorWrites[0].dstArrayElement = 0;
		descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
		descriptorWrites[0].descriptorCount = 1;
		descriptorWrites[0].pBufferInfo = &mesh_perframe_storage_buffer;

		descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrites[1].pNext = NULL;
		descriptorWrites[1].dstSet = *m_descriptors[axis].descriptor_set;
		descriptorWrites[1].dstBinding = 1;
		descriptorWrites[1].dstArrayElement = 0;
		descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
		descriptorWrites[1].descriptorCount = 1;
		descriptorWrites[1].pBufferInfo = &axis_storage_buffer;


		vkUpdateDescriptorSets(m_vulkan_rhi->getDevice(), static_cast<uint32_t>(sizeof(descriptorWrites) / sizeof(descriptorWrites[0])), descriptorWrites, 0, nullptr);
	}


	void MainCameraRenderPass::setupSkyboxDescriptorSet()
	{
		VkDescriptorSetAllocateInfo descriptor_set_allocate_info{};
		descriptor_set_allocate_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		descriptor_set_allocate_info.descriptorPool = m_vulkan_rhi->getDescriptorPool();
		descriptor_set_allocate_info.descriptorSetCount = 1;
		descriptor_set_allocate_info.pSetLayouts = m_descriptors[skybox].descriptor_set_layout;
		descriptor_set_allocate_info.pNext = nullptr;
		m_vulkan_rhi->allocateDescriptorSets(m_descriptors[skybox].descriptor_set_layout, 1, m_descriptors[skybox].descriptor_set);

		VkDescriptorImageInfo specular_image_info{};
		specular_image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		specular_image_info.imageView = m_global_render_resource->ibl_resource.specular_texture_image_view;
		specular_image_info.sampler = m_global_render_resource->ibl_resource.specular_texture_image_sampler;

		VkDescriptorImageInfo	base_color_input_attachment{};
		base_color_input_attachment.imageView = m_framebuffer.attachments[main_camera_pass_base_color_attachment].image_view;
		base_color_input_attachment.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		base_color_input_attachment.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice());

		VkDescriptorImageInfo	depth_input_attachment{};
		depth_input_attachment.imageView = m_framebuffer.attachments[main_camera_pass_depth_attachment].image_view;
		depth_input_attachment.sampler = 		base_color_input_attachment.sampler = m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice());
		m_vulkan_rhi->getOrCreateNearestSampler(m_vulkan_rhi->getPhysicalDevice(), m_vulkan_rhi->getDevice());
		depth_input_attachment.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkWriteDescriptorSet write_descriptor_set[3];
		write_descriptor_set[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		write_descriptor_set[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[0].dstSet = *m_descriptors[skybox].descriptor_set;
		write_descriptor_set[0].dstBinding = 0;
		write_descriptor_set[0].descriptorCount = 1;
		write_descriptor_set[0].dstArrayElement = 0;
		write_descriptor_set[0].pImageInfo = &specular_image_info;
		write_descriptor_set[0].pNext = nullptr;

		write_descriptor_set[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[1].pNext = NULL;
		write_descriptor_set[1].dstSet = *m_descriptors[skybox].descriptor_set;
		write_descriptor_set[1].dstBinding = 1;
		write_descriptor_set[1].dstArrayElement = 0;
		write_descriptor_set[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		write_descriptor_set[1].descriptorCount = 1;
		write_descriptor_set[1].pImageInfo = &base_color_input_attachment;

		write_descriptor_set[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		write_descriptor_set[2].pNext = NULL;
		write_descriptor_set[2].dstSet = *m_descriptors[skybox].descriptor_set;
		write_descriptor_set[2].dstBinding = 2;
		write_descriptor_set[2].dstArrayElement = 0;
		write_descriptor_set[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
		write_descriptor_set[2].descriptorCount = 1;
		write_descriptor_set[2].pImageInfo = &depth_input_attachment;

		vkUpdateDescriptorSets(m_vulkan_rhi->getDevice(), (sizeof(write_descriptor_set) / sizeof(write_descriptor_set[0])), write_descriptor_set, 0, nullptr);
	}

	void MainCameraRenderPass::updateAfterRecreateSwapChain()
	{
		for (size_t i = 0; i < m_framebuffer.attachments.size(); ++i)
		{
			vkDestroyImage(m_vulkan_rhi->getDevice(), m_framebuffer.attachments[i].image, nullptr);
			vkDestroyImageView(m_vulkan_rhi->getDevice(), m_framebuffer.attachments[i].image_view, nullptr);
			vkFreeMemory(m_vulkan_rhi->getDevice(), m_framebuffer.attachments[i].memory, nullptr);
		}

		for (size_t i = 0; i < m_swapchain_framebuffers.size(); ++i)
		{
			vkDestroyFramebuffer(m_vulkan_rhi->getDevice(), m_swapchain_framebuffers[i], nullptr);
		}

		setupAttachments();
		//setupRenderPass();
		setupDescriptorSets();
		setupSwapChainFrameBuffers();
	}

	void MainCameraRenderPass::setVisibleAxis(bool state)
	{
		m_axis_show = state;
	}

	void MainCameraRenderPass::drawAxis()
	{
		if (!m_axis_show) return;
		uint32_t current_frame_index = m_vulkan_rhi->getCurrentFrameIndex();
		uint32_t perframe_storage_buffer_offset = roundUp(m_global_render_resource->storage_buffer.ringbuffer_end[current_frame_index], m_global_render_resource->storage_buffer.min_storage_buffer_offset_alignment);
		m_global_render_resource->storage_buffer.ringbuffer_end[current_frame_index] = perframe_storage_buffer_offset + sizeof(MeshPerFrameStorageBufferObject);
		assert(m_global_render_resource->storage_buffer.ringbuffer_end[current_frame_index] <= m_global_render_resource->storage_buffer.ringbuffer_begin[current_frame_index] + m_global_render_resource->storage_buffer.ringbuffer_size[current_frame_index]);
		(*reinterpret_cast<MeshPerFrameStorageBufferObject*>(reinterpret_cast<uintptr_t>(m_global_render_resource->storage_buffer.global_ringbuffer_memory_pointer) + perframe_storage_buffer_offset)) = m_mesh_perframe_storage_buffer_object;

		m_axis_storage_buffer_object.model_matrix = m_visible_render_meshes.p_axis_node->model_matrix;

		m_axis_storage_buffer_object.selected_axis = m_selected_axis;

		(*reinterpret_cast<AxisStorageBufferObject*>(reinterpret_cast<uintptr_t>(m_global_render_resource->storage_buffer.axis_storage_buffer_memory_pointer))) = m_axis_storage_buffer_object;

		uint32_t dynamic_offsets[1] = { perframe_storage_buffer_offset };
		vkCmdBindPipeline(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_axis].pipeline);
		vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_axis].pipeline_layout, 0, 1, m_descriptors[axis].descriptor_set, (sizeof(dynamic_offsets) / sizeof(dynamic_offsets[0])), dynamic_offsets);

		VkBuffer vertex_buffers[] = { m_visible_render_meshes.p_axis_node->p_mesh->mesh_vertex_position_buffer,
		m_visible_render_meshes.p_axis_node->p_mesh->mesh_vertex_normal_buffer,
		m_visible_render_meshes.p_axis_node->p_mesh->mesh_vertex_tangent_buffer,
		m_visible_render_meshes.p_axis_node->p_mesh->mesh_vertex_uv_buffer };
		VkDeviceSize offsets[] = { 0,0,0,0 };
		vkCmdBindVertexBuffers(m_vulkan_rhi->getCurrentCommandBuffer(), 0, (sizeof(vertex_buffers) / sizeof(vertex_buffers[0])), vertex_buffers, offsets);
		vkCmdBindIndexBuffer(m_vulkan_rhi->getCurrentCommandBuffer(), m_visible_render_meshes.p_axis_node->p_mesh->mesh_vertex_index_buffer, 0, VK_INDEX_TYPE_UINT16);
		vkCmdDrawIndexed(m_vulkan_rhi->getCurrentCommandBuffer(), m_visible_render_meshes.p_axis_node->p_mesh->index_count, 1, 0, 0, 0);
	}

	void MainCameraRenderPass::prepareData(std::shared_ptr<RenderResourceBase> resource)
	{
		RenderResource* m_resource = static_cast<RenderResource*>(resource.get());
		if (m_resource)
		{
			m_mesh_perframe_storage_buffer_object = m_resource->m_mesh_per_frame_storage_buffer_object;

		}
	}

	void MainCameraRenderPass::setupRenderPass()
	{
		VkAttachmentDescription attachments[main_camera_pass_attachment_count] = {};
		VkAttachmentDescription& base_color_attachment = attachments[main_camera_pass_base_color_attachment];
		base_color_attachment.format = m_vulkan_rhi->getSwapChainImageFormat();
		base_color_attachment.samples = m_vulkan_rhi->getMSAASamples();
		base_color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		base_color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		base_color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		base_color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		base_color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		base_color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;


		VkAttachmentDescription& depth_attachment = attachments[main_camera_pass_depth_attachment];
		depth_attachment.format = m_vulkan_rhi->getDepthImageFormat();
		depth_attachment.samples = m_vulkan_rhi->getMSAASamples();
		depth_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		depth_attachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		depth_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		depth_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		depth_attachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		//VkAttachmentDescription& skybox_attachment = attachments[main_camera_pass_skybox_attachment];
		//skybox_attachment.format = m_vulkan_rhi->getSwapChainImageFormat();
		//skybox_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		//skybox_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		//skybox_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		//skybox_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		//skybox_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		//skybox_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		//skybox_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription& backup_odd_color_attachment = attachments[main_camera_pass_backup_odd_color_attachment];
		backup_odd_color_attachment.format = m_vulkan_rhi->getSwapChainImageFormat();
		backup_odd_color_attachment.samples = m_vulkan_rhi->getMSAASamples();
		backup_odd_color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		backup_odd_color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		backup_odd_color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		backup_odd_color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		backup_odd_color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		backup_odd_color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription& backup_even_color_attachment = attachments[main_camera_pass_backup_even_color_attachment];
		backup_even_color_attachment.format = m_vulkan_rhi->getSwapChainImageFormat();
		backup_even_color_attachment.samples = m_vulkan_rhi->getMSAASamples();
		backup_even_color_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		backup_even_color_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		backup_even_color_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		backup_even_color_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		backup_even_color_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		backup_even_color_attachment.finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentDescription& swap_chain_attachment = attachments[main_camera_pass_swap_chain_image_attachment];
		swap_chain_attachment.format = m_vulkan_rhi->getSwapChainImageFormat();
		swap_chain_attachment.samples = VK_SAMPLE_COUNT_1_BIT;
		swap_chain_attachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		swap_chain_attachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		swap_chain_attachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		swap_chain_attachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		swap_chain_attachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		swap_chain_attachment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		VkAttachmentReference base_color_attachment_ref{};
		base_color_attachment_ref.attachment = &base_color_attachment - attachments;
		base_color_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depth_attachment_ref{};
		depth_attachment_ref.attachment = &depth_attachment - attachments;
		depth_attachment_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference skybox_color_attachment_reference{};
		skybox_color_attachment_reference.attachment = &backup_odd_color_attachment - attachments;
		skybox_color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		VkSubpassDescription subpass[main_camera_subpass_count] = {};

		//base subpass
		VkSubpassDescription& main_camera_base_subpass = subpass[main_camera_subpass_basepass];
		main_camera_base_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		main_camera_base_subpass.colorAttachmentCount = 1;
		main_camera_base_subpass.pColorAttachments = &base_color_attachment_ref;
		main_camera_base_subpass.pDepthStencilAttachment = &depth_attachment_ref;
		//main_camera_base_subpass.pResolveAttachments = &swap_chain_attachment_ref;

		VkAttachmentReference skybox_input_attachment_references[2];
		VkAttachmentReference& skybox_base_input_attachment_ref = skybox_input_attachment_references[0];
		skybox_base_input_attachment_ref.attachment = &base_color_attachment - attachments;
		skybox_base_input_attachment_ref.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference& skybox_depth_input_attachment_ref = skybox_input_attachment_references[1];
		skybox_depth_input_attachment_ref.attachment = &depth_attachment - attachments;
		skybox_depth_input_attachment_ref.layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		//VkAttachmentReference skyBox_attachment_reference{};
		//skyBox_attachment_reference.attachment = &swap_chain_attachment - attachments;
		//skyBox_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//VkAttachmentReference swap_chain_attachment_ref{};
		//swap_chain_attachment_ref.attachment = &swap_chain_attachment - attachments;
		//swap_chain_attachment_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//skybox subpass
		VkSubpassDescription& main_camera_skybox_subpass = subpass[main_camera_subpass_skybox];
		main_camera_skybox_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		main_camera_skybox_subpass.inputAttachmentCount = (sizeof(skybox_input_attachment_references) / sizeof(skybox_input_attachment_references[0]));
		main_camera_skybox_subpass.pInputAttachments = skybox_input_attachment_references;
		main_camera_skybox_subpass.colorAttachmentCount = 1;
		main_camera_skybox_subpass.pColorAttachments = &skybox_color_attachment_reference;


		VkAttachmentReference ui_subpass_color_attachment_reference;
		ui_subpass_color_attachment_reference.attachment = &backup_even_color_attachment - attachments;
		ui_subpass_color_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		//ui subpass
		VkSubpassDescription& main_camera_axis_subpass = subpass[main_camera_subpass_ui];
		main_camera_axis_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		main_camera_axis_subpass.inputAttachmentCount = 0;
		main_camera_axis_subpass.pInputAttachments = nullptr;
		main_camera_axis_subpass.colorAttachmentCount = 1;
		main_camera_axis_subpass.pColorAttachments = &ui_subpass_color_attachment_reference;

		VkAttachmentReference combine_ui_subpass_input_attachments[2];
		combine_ui_subpass_input_attachments[0].attachment = &backup_odd_color_attachment - attachments;
		combine_ui_subpass_input_attachments[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		combine_ui_subpass_input_attachments[1].attachment = &backup_even_color_attachment - attachments;
		combine_ui_subpass_input_attachments[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		VkAttachmentReference swap_chain_attachment_reference;
		swap_chain_attachment_reference.attachment = &swap_chain_attachment - attachments;
		swap_chain_attachment_reference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		//combine ui
		VkSubpassDescription& main_camera_combine_ui_subpass = subpass[main_camera_subpass_combine_ui];
		main_camera_combine_ui_subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		main_camera_combine_ui_subpass.inputAttachmentCount = sizeof(combine_ui_subpass_input_attachments) / sizeof(combine_ui_subpass_input_attachments[0]);
		main_camera_combine_ui_subpass.pInputAttachments = combine_ui_subpass_input_attachments;
		main_camera_combine_ui_subpass.colorAttachmentCount = 1;
		main_camera_combine_ui_subpass.pColorAttachments = &swap_chain_attachment_reference;


		VkSubpassDependency dependency[main_camera_subpass_dependency_count] = {};
		VkSubpassDependency& base_dependency = dependency[main_camera_subpass_base_dependency];
		base_dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		base_dependency.dstSubpass = main_camera_subpass_basepass;
		base_dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		base_dependency.srcAccessMask = 0;
		base_dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
		base_dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		VkSubpassDependency& skybox_pass_depend_on_base_pass = dependency[main_camera_subpass_skybox_dependency];
		skybox_pass_depend_on_base_pass.srcSubpass = main_camera_subpass_basepass;
		skybox_pass_depend_on_base_pass.dstSubpass = main_camera_subpass_skybox;
		skybox_pass_depend_on_base_pass.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		skybox_pass_depend_on_base_pass.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		skybox_pass_depend_on_base_pass.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		skybox_pass_depend_on_base_pass.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		skybox_pass_depend_on_base_pass.dependencyFlags = 0;

		VkSubpassDependency& ui_pass_depend_on_skybox_pass = dependency[main_camera_subpass_dependency_ui];
		ui_pass_depend_on_skybox_pass.srcSubpass = main_camera_subpass_skybox;
		ui_pass_depend_on_skybox_pass.dstSubpass = main_camera_subpass_ui;
		ui_pass_depend_on_skybox_pass.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		ui_pass_depend_on_skybox_pass.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		ui_pass_depend_on_skybox_pass.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		ui_pass_depend_on_skybox_pass.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		ui_pass_depend_on_skybox_pass.dependencyFlags = 0;

		VkSubpassDependency& combine_ui_pass_depend_on_ui_pass = dependency[main_camera_subpass_dependency_combine_ui];
		combine_ui_pass_depend_on_ui_pass.srcSubpass = main_camera_subpass_ui;
		combine_ui_pass_depend_on_ui_pass.dstSubpass = main_camera_subpass_combine_ui;
		combine_ui_pass_depend_on_ui_pass.srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		combine_ui_pass_depend_on_ui_pass.dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		combine_ui_pass_depend_on_ui_pass.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		combine_ui_pass_depend_on_ui_pass.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_READ_BIT;
		combine_ui_pass_depend_on_ui_pass.dependencyFlags = 0;


		VkRenderPassCreateInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = (sizeof(attachments) / sizeof(attachments[0]));
		renderPassInfo.pAttachments = attachments;
		renderPassInfo.subpassCount = (sizeof(subpass) / sizeof(subpass[0]));
		renderPassInfo.pSubpasses = subpass;
		renderPassInfo.dependencyCount = (sizeof(dependency) / sizeof(dependency[0]));
		renderPassInfo.pDependencies = dependency;

		if (vkCreateRenderPass(m_vulkan_rhi->getDevice(), &renderPassInfo, nullptr, &m_framebuffer.render_pass))
		{
			throw std::runtime_error("failed to create render pass");
		}
	}




	void setupFrameBuffer()
	{

	}

	void MainCameraRenderPass::setupDescriptorSetLayout()
	{
		m_descriptors.resize(descriptor_set_layout_type_count);
		{
			VkDescriptorSetLayoutBinding mesh_global_layout_bindings[3];

			VkDescriptorSetLayoutBinding& mesh_global_layout_mesh_perframe_storage_buffer_binding = mesh_global_layout_bindings[0];
			mesh_global_layout_mesh_perframe_storage_buffer_binding.binding = 1;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.descriptorCount = 1;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.pImmutableSamplers = nullptr;
			mesh_global_layout_mesh_perframe_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutBinding& mesh_global_layout_mesh_perdrawcall_storage_buffer_binding = mesh_global_layout_bindings[1];
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.binding = 2;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.descriptorCount = 1;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.pImmutableSamplers = nullptr;
			mesh_global_layout_mesh_perdrawcall_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutBinding& mesh_global_layout_mesh_vertex_joint_blending_binding = mesh_global_layout_bindings[2];
			mesh_global_layout_mesh_vertex_joint_blending_binding.binding = 3;
			mesh_global_layout_mesh_vertex_joint_blending_binding.descriptorCount = 1;
			mesh_global_layout_mesh_vertex_joint_blending_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			mesh_global_layout_mesh_vertex_joint_blending_binding.pImmutableSamplers = nullptr;
			mesh_global_layout_mesh_vertex_joint_blending_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;

			VkDescriptorSetLayoutCreateInfo layoutInfo{};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = (sizeof(mesh_global_layout_bindings) / sizeof(mesh_global_layout_bindings[0]));
			layoutInfo.pBindings = mesh_global_layout_bindings;
			m_vulkan_rhi->createDescriptorSetLayout(&layoutInfo, nullptr, m_descriptors[global_mesh].descriptor_set_layout);
			
		}

		{
			//per mesh
			VkDescriptorSetLayoutBinding per_mesh_layout_binding[1];
			VkDescriptorSetLayoutBinding& per_mesh_vertex_blending_binding = per_mesh_layout_binding[0];
			per_mesh_vertex_blending_binding.binding = 0;
			per_mesh_vertex_blending_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			per_mesh_vertex_blending_binding.descriptorCount = 1;
			per_mesh_vertex_blending_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			per_mesh_vertex_blending_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo per_mesh_layout_create_info;
			per_mesh_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			per_mesh_layout_create_info.pNext = NULL;
			per_mesh_layout_create_info.flags = 0;
			per_mesh_layout_create_info.bindingCount = (sizeof(per_mesh_layout_binding) / sizeof(per_mesh_layout_binding[0]));
			per_mesh_layout_create_info.pBindings = per_mesh_layout_binding;

			m_vulkan_rhi->createDescriptorSetLayout(&per_mesh_layout_create_info, nullptr, m_descriptors[per_mesh].descriptor_set_layout);
		}

		{
			//mesh per material descriptor set layout
			VkDescriptorSetLayoutBinding mesh_per_material_layout_binding[1];
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
			mesh_material_layout_create_info.bindingCount = (sizeof(mesh_per_material_layout_binding) / sizeof(mesh_per_material_layout_binding[0]));
			mesh_material_layout_create_info.pBindings = mesh_per_material_layout_binding;

			m_vulkan_rhi->createDescriptorSetLayout(&mesh_material_layout_create_info,
				nullptr,
				m_descriptors[mesh_per_material].descriptor_set_layout);
		}

		{
			//skybox descriptor set layout out
			VkDescriptorSetLayoutBinding skybox_descriptor_set_layout_bindings[3];
			VkDescriptorSetLayoutBinding& skybox_descriptor_set_layout_cube_sampler_binding = skybox_descriptor_set_layout_bindings[0];
			skybox_descriptor_set_layout_cube_sampler_binding.binding = 0;
			skybox_descriptor_set_layout_cube_sampler_binding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			skybox_descriptor_set_layout_cube_sampler_binding.descriptorCount = 1;
			skybox_descriptor_set_layout_cube_sampler_binding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
			skybox_descriptor_set_layout_cube_sampler_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding& base_color_input_attachment = skybox_descriptor_set_layout_bindings[1];
			base_color_input_attachment.binding = 1;
			base_color_input_attachment.descriptorCount = 1;
			base_color_input_attachment.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			base_color_input_attachment.pImmutableSamplers = nullptr;
			base_color_input_attachment.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutBinding& depth_input_attachment = skybox_descriptor_set_layout_bindings[2];
			depth_input_attachment.binding = 2;
			depth_input_attachment.descriptorCount = 1;
			depth_input_attachment.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			depth_input_attachment.pImmutableSamplers = nullptr;
			depth_input_attachment.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;

			VkDescriptorSetLayoutCreateInfo skybox_layout_create_info{};
			skybox_layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			skybox_layout_create_info.bindingCount = (sizeof(skybox_descriptor_set_layout_bindings) / sizeof(skybox_descriptor_set_layout_bindings[0]));
			skybox_layout_create_info.pBindings = skybox_descriptor_set_layout_bindings;
			skybox_layout_create_info.flags = 0;
			skybox_layout_create_info.pNext = nullptr;
			m_vulkan_rhi->createDescriptorSetLayout(&skybox_layout_create_info, nullptr, m_descriptors[skybox].descriptor_set_layout);
			
		}
		//ui pass descriptor set layout
		{
			VkDescriptorSetLayoutBinding axis_descriptor_set_layout_bindings[2];
			VkDescriptorSetLayoutBinding& perframe_storage_buffer_binding = axis_descriptor_set_layout_bindings[0];
			perframe_storage_buffer_binding.binding = 0;
			perframe_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			perframe_storage_buffer_binding.descriptorCount = 1;
			perframe_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			perframe_storage_buffer_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding& axis_storage_buffer_binding = axis_descriptor_set_layout_bindings[1];
			axis_storage_buffer_binding.binding = 1;
			axis_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			axis_storage_buffer_binding.descriptorCount = 1;
			axis_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			axis_storage_buffer_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layout_create_info{};
			layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_create_info.bindingCount = sizeof(axis_descriptor_set_layout_bindings) / sizeof(axis_descriptor_set_layout_bindings[0]);
			layout_create_info.pBindings = axis_descriptor_set_layout_bindings;
			layout_create_info.pNext = nullptr;

			m_vulkan_rhi->createDescriptorSetLayout(&layout_create_info, nullptr, m_descriptors[axis].descriptor_set_layout);
		}

		//combine ui pass descriptor set layout
		{
			VkDescriptorSetLayoutBinding axis_descriptor_set_layout_bindings[2];
			VkDescriptorSetLayoutBinding& perframe_storage_buffer_binding = axis_descriptor_set_layout_bindings[0];
			perframe_storage_buffer_binding.binding = 0;
			perframe_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC;
			perframe_storage_buffer_binding.descriptorCount = 1;
			perframe_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			perframe_storage_buffer_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutBinding& axis_storage_buffer_binding = axis_descriptor_set_layout_bindings[1];
			axis_storage_buffer_binding.binding = 1;
			axis_storage_buffer_binding.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
			axis_storage_buffer_binding.descriptorCount = 1;
			axis_storage_buffer_binding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			axis_storage_buffer_binding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layout_create_info{};
			layout_create_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layout_create_info.bindingCount = sizeof(axis_descriptor_set_layout_bindings) / sizeof(axis_descriptor_set_layout_bindings[0]);
			layout_create_info.pBindings = axis_descriptor_set_layout_bindings;
			layout_create_info.pNext = nullptr;

			m_vulkan_rhi->createDescriptorSetLayout(&layout_create_info, nullptr, m_descriptors[axis].descriptor_set_layout);
			
		}
	}

	void MainCameraRenderPass::setupRenderPipelines()
	{
		m_render_pipelines.resize(render_pipeline_type_count);
		//mesh global buffer render pipeline
		{

			VkDescriptorSetLayout  descriptorSetLayout[3] = { *m_descriptors[global_mesh].descriptor_set_layout,*m_descriptors[per_mesh].descriptor_set_layout ,*m_descriptors[mesh_per_material].descriptor_set_layout };
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			uint32_t descriptorset_layout_count = static_cast<uint32_t>(sizeof(descriptorSetLayout) / sizeof(descriptorSetLayout[0]));
			pipelineLayoutInfo.setLayoutCount = descriptorset_layout_count;
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

			
			if (vkCreatePipelineLayout(m_vulkan_rhi->getDevice(), &pipelineLayoutInfo, nullptr, &m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			auto vertShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//common_mesh_vert.spv");
			auto fragShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//common_mesh_frag.spv");

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

			auto bindingDescription = MeshVertex::getBindingDescription();
			auto attributeDescriptions = MeshVertex::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
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

		//skybox
		{
			VkDescriptorSetLayout descriptor_set_layouts[2] = { *m_descriptors[global_mesh].descriptor_set_layout,
																*m_descriptors[skybox].descriptor_set_layout };
			VkPipelineLayout pipeline_layout;
			VkPipelineLayoutCreateInfo create_info{};
			create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			create_info.setLayoutCount = sizeof(descriptor_set_layouts) / sizeof(descriptor_set_layouts[0]);
			create_info.pSetLayouts = descriptor_set_layouts;

			if (vkCreatePipelineLayout(m_vulkan_rhi->getDevice(), &create_info, nullptr, &m_render_pipelines[render_pipeline_type_skybox].pipeline_layout) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create pipeline layout !");
			}

			auto vertShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//skybox_vert.spv");
			auto fragShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//skybox_frag.spv");

			VkShaderModule vertex_shader_module = m_vulkan_rhi->createShaderModule(m_vulkan_rhi->getDevice(), vertShaderCode);
			VkShaderModule fragment_shader_module = m_vulkan_rhi->createShaderModule(m_vulkan_rhi->getDevice(), fragShaderCode);

			VkPipelineShaderStageCreateInfo vertex_shader_shtage_info = {};
			vertex_shader_shtage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			vertex_shader_shtage_info.stage = VK_SHADER_STAGE_VERTEX_BIT;
			vertex_shader_shtage_info.pName = "main";
			vertex_shader_shtage_info.module = vertex_shader_module;

			VkPipelineShaderStageCreateInfo fragment_shader_shtage_info = {};
			fragment_shader_shtage_info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
			fragment_shader_shtage_info.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
			fragment_shader_shtage_info.pName = "main";
			fragment_shader_shtage_info.module = fragment_shader_module;

			VkPipelineShaderStageCreateInfo shader_stages[2] = { vertex_shader_shtage_info,fragment_shader_shtage_info };


			VkVertexInputBindingDescription binding_description;
			binding_description.binding = 0;
			binding_description.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			binding_description.stride = sizeof(Vector3);

			VkVertexInputAttributeDescription attrib_description;
			attrib_description.binding = 0;
			attrib_description.location = 0;
			attrib_description.offset = 0;
			attrib_description.format = VK_FORMAT_R32G32B32_SFLOAT;

			VkPipelineVertexInputStateCreateInfo vertex_input_state_info{};
			vertex_input_state_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vertex_input_state_info.vertexBindingDescriptionCount = 0;
			vertex_input_state_info.pVertexBindingDescriptions = nullptr;
			vertex_input_state_info.vertexAttributeDescriptionCount = 0;
			vertex_input_state_info.pVertexAttributeDescriptions = nullptr;

			VkPipelineInputAssemblyStateCreateInfo input_assembly{};
			input_assembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			input_assembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
			input_assembly.primitiveRestartEnable = VK_FALSE;

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

			VkPipelineMultisampleStateCreateInfo multisample_state_create_info{};
			multisample_state_create_info.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			multisample_state_create_info.sampleShadingEnable = VK_FALSE;
			multisample_state_create_info.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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

			VkGraphicsPipelineCreateInfo pipeline_info{};
			pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			pipeline_info.stageCount = 2;
			pipeline_info.pStages = shader_stages;
			pipeline_info.pVertexInputState = &vertex_input_state_info;
			pipeline_info.pInputAssemblyState = &input_assembly;
			pipeline_info.pViewportState = &viewportState;
			pipeline_info.pRasterizationState = &rasterizer;
			pipeline_info.pMultisampleState = &multisample_state_create_info;
			pipeline_info.pDepthStencilState = &depthStencil;
			pipeline_info.pColorBlendState = &colorBlending;
			pipeline_info.pDynamicState = &dynamicState;
			pipeline_info.layout = m_render_pipelines[render_pipeline_type_skybox].pipeline_layout;
			pipeline_info.renderPass = m_framebuffer.render_pass;
			pipeline_info.subpass = main_camera_subpass_skybox;
			pipeline_info.basePipelineHandle = VK_NULL_HANDLE;


			if (vkCreateGraphicsPipelines(m_vulkan_rhi->getDevice(), VK_NULL_HANDLE, 1, &pipeline_info, nullptr, &m_render_pipelines[render_pipeline_type_skybox].pipeline) != VK_SUCCESS)
			{
				throw std::runtime_error("failed to create graphics pipeline!");
			}
			vkDestroyShaderModule(m_vulkan_rhi->getDevice(), vertex_shader_module, nullptr);
			vkDestroyShaderModule(m_vulkan_rhi->getDevice(), fragment_shader_module, nullptr);
		}

		//axis pipeline
		{
			VkDescriptorSetLayout  descriptorSetLayout[1] = { *m_descriptors[axis].descriptor_set_layout };
			VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
			pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			uint32_t descriptorset_layout_count = static_cast<uint32_t>(sizeof(descriptorSetLayout) / sizeof(descriptorSetLayout[0]));
			pipelineLayoutInfo.setLayoutCount = descriptorset_layout_count;
			pipelineLayoutInfo.pSetLayouts = descriptorSetLayout;

			if (vkCreatePipelineLayout(m_vulkan_rhi->getDevice(), &pipelineLayoutInfo, nullptr, &m_render_pipelines[render_pipeline_type_axis].pipeline_layout) != VK_SUCCESS) {
				throw std::runtime_error("failed to create pipeline layout!");
			}

			auto vertShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//axis_vert.spv");
			auto fragShaderCode = Utils::readFile("E://VS_Project//QyhsEngine//engine//shader//axis_frag.spv");

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

			auto bindingDescription = MeshVertex::getBindingDescription();
			auto attributeDescriptions = MeshVertex::getAttributeDescriptions();

			vertexInputInfo.vertexBindingDescriptionCount = static_cast<uint32_t>(bindingDescription.size());
			vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
			vertexInputInfo.pVertexBindingDescriptions = bindingDescription.data();
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
			pipelineInfo.layout = m_render_pipelines[render_pipeline_type_axis].pipeline_layout;
			pipelineInfo.renderPass = m_framebuffer.render_pass;
			pipelineInfo.subpass = main_camera_subpass_ui;
			pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

			if (vkCreateGraphicsPipelines(m_vulkan_rhi->getDevice(), VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_render_pipelines[render_pipeline_type_axis].pipeline) != VK_SUCCESS) {
				throw std::runtime_error("failed to create graphics pipeline!");
			}

			vkDestroyShaderModule(m_vulkan_rhi->getDevice(), fragShaderModule, nullptr);
			vkDestroyShaderModule(m_vulkan_rhi->getDevice(), vertShaderModule, nullptr);
		}
	}


	void MainCameraRenderPass::setupDescriptorSets()
	{
		setupGlobalModelDescriptorSet();
		setupSkyboxDescriptorSet();
		setAxisDescriptorSet();
	}

	void MainCameraRenderPass::setupSwapChainFrameBuffers()
	{
		m_swapchain_framebuffers.resize(m_vulkan_rhi->getSwapChainImageView().size());

		for (size_t i = 0; i < m_vulkan_rhi->getSwapChainImageView().size(); i++) {
			std::array<VkImageView, main_camera_pass_attachment_count> attachments = {
				m_framebuffer.attachments[main_camera_pass_base_color_attachment].image_view,
				m_framebuffer.attachments[main_camera_pass_depth_attachment].image_view,
				m_framebuffer.attachments[main_camera_pass_backup_odd_color_attachment].image_view,
				m_framebuffer.attachments[main_camera_pass_backup_even_color_attachment].image_view,
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

	void MainCameraRenderPass::draw(UIPass& ui_pass, CombineUIPass& combine_ui_pass)
	{
		VkRenderPassBeginInfo renderPassInfo{};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_framebuffer.render_pass;
		renderPassInfo.framebuffer = m_swapchain_framebuffers[m_vulkan_rhi->getCurrentFrameIndex()];
		renderPassInfo.renderArea.offset = { 0, 0 };
		renderPassInfo.renderArea.extent = m_vulkan_rhi->getSwapChainExtent();

		std::array<VkClearValue, main_camera_pass_attachment_count> clearValues{};
		clearValues[0].color = { {1.0f, 1.0f, 1.0f, 1.0f} };
		clearValues[1].depthStencil = { 1.0f, 0 };
		//clearValues[main_camera_pass_skybox_attachment].color = { {1.0f,1.0f,1.0f,1.0f} };
		renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(m_vulkan_rhi->getCurrentCommandBuffer(), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		drawMesh();
		vkCmdNextSubpass(m_vulkan_rhi->getCurrentCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
		m_vulkan_rhi->beginEvent(m_vulkan_rhi->getCurrentCommandBuffer(), "Skybox");
		drawSkyBox();
		m_vulkan_rhi->endEvent(m_vulkan_rhi->getCurrentCommandBuffer());
		vkCmdNextSubpass(m_vulkan_rhi->getCurrentCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
		m_vulkan_rhi->beginEvent(m_vulkan_rhi->getCurrentCommandBuffer(), "Axis");
		drawAxis();
		m_vulkan_rhi->endEvent(m_vulkan_rhi->getCurrentCommandBuffer());
		ui_pass.draw();
		vkCmdNextSubpass(m_vulkan_rhi->getCurrentCommandBuffer(), VK_SUBPASS_CONTENTS_INLINE);
		m_vulkan_rhi->beginEvent(m_vulkan_rhi->getCurrentCommandBuffer(), "Combine UI");
		combine_ui_pass.draw();
		m_vulkan_rhi->endEvent(m_vulkan_rhi->getCurrentCommandBuffer());
		vkCmdEndRenderPass(m_vulkan_rhi->getCurrentCommandBuffer());
	}

	void MainCameraRenderPass::drawMesh()
	{
		struct MeshNode
		{
			Matrix4x4 *model_matrix{ nullptr };
			Matrix4x4* joint_matrices{ nullptr };
			size_t joint_count{ 0 };
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
				tmp.model_matrix = &node.model_matrix;
				if (node.enable_vertex_blending)
				{
					tmp.joint_matrices = node.joint_matrices;
					tmp.joint_count = node.joint_count;
				}
				mesh_nodes.push_back(tmp);
			}

			vkCmdBindPipeline(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[global_mesh].pipeline);

			vkCmdSetViewport(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &m_vulkan_rhi->getViewport());

			VkRect2D scissor{};
			scissor.offset = { 0, 0 };
			scissor.extent = m_vulkan_rhi->getSwapChainExtent();
			vkCmdSetScissor(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &scissor);

			for (auto& material_map_to_meshinstance : main_camera_pass_mesh_batch)
			{
				VulkanMaterial& material = *(material_map_to_meshinstance.first);

				auto& mesh_instance = material_map_to_meshinstance.second;

				//material.material_descriptor_set = m_descriptors[per_mesh].descriptor_set;
				vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
					m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout, 2, 1,
					material.material_descriptor_set, 0, nullptr);
				for (auto& pair : mesh_instance)
				{
					auto& mesh = *(pair.first);
					auto& mesh_nodes = pair.second;
					uint32_t total_instance_count = mesh_nodes.size();
					if (total_instance_count > 0)
					{
						m_vulkan_rhi->cmdBindDescriptorSets(VK_PIPELINE_BIND_POINT_GRAPHICS,
							&m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout,1,1,&mesh.p_vertex_blending_descriptor_set,0,nullptr);
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
									*mesh_nodes[drawcall_max_instance_count * drawcall_index + i].model_matrix;
								perdrawcall_storage_buffer_object.mesh_instances[i].enable_vertex_blending =
									mesh_nodes[drawcall_max_instance_count * drawcall_index + i].joint_matrices ? 1.0 : -1.0;
							}

							//per drawcall vertex blending storage buffer
							bool enable_vertex_blending = true;
							uint32_t perdrawcall_vertex_blending_offset;
							for (size_t i = 0; i < current_instance_count; ++i)
							{
								if (!mesh_nodes[drawcall_max_instance_count * drawcall_index + i].joint_matrices)
								{
									enable_vertex_blending = false;
									break;
								}
							}
							if (enable_vertex_blending)
							{
								perdrawcall_vertex_blending_offset =
									roundUp(m_global_render_resource->storage_buffer
										.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()],
										m_global_render_resource->storage_buffer.min_storage_buffer_offset_alignment);
								m_global_render_resource->storage_buffer
									.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] =
									perdrawcall_vertex_blending_offset + sizeof(MeshPerdrawcallVertexBlendingStorageBufferObject);
								assert(m_global_render_resource->storage_buffer
									.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] <=
									(m_global_render_resource->storage_buffer
										.ringbuffer_begin[m_vulkan_rhi->getCurrentFrameIndex()] +
										m_global_render_resource->storage_buffer
										.ringbuffer_size[m_vulkan_rhi->getCurrentFrameIndex()]));
								MeshPerdrawcallVertexBlendingStorageBufferObject& perdrawcall_vertex_blending_storage_buffer_object =
									(*reinterpret_cast<MeshPerdrawcallVertexBlendingStorageBufferObject*>(
										reinterpret_cast<uintptr_t>(m_global_render_resource->storage_buffer
											.global_ringbuffer_memory_pointer) +
										perdrawcall_vertex_blending_offset));
								for (uint32_t index = 0; index < current_instance_count; ++index)
								{
									if (mesh_nodes[drawcall_max_instance_count * drawcall_index + index].joint_matrices)
									{
										for (uint32_t j = 0; j < mesh_nodes[drawcall_max_instance_count * drawcall_index + index].joint_count; ++j)
										{
											perdrawcall_vertex_blending_storage_buffer_object.joint_matrices[s_mesh_vertex_blending_max_joint_count * index + j]
												= mesh_nodes[drawcall_max_instance_count * drawcall_index + index].joint_matrices[j];
										}
									}
								}
							}
							else
							{
								perdrawcall_vertex_blending_offset = 0;
							}

							std::array<uint32_t, 3> dynamic_offsets = { perframe_dynamic_offset,perdrawcall_dynamic_offset ,perdrawcall_vertex_blending_offset };
							vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(),
								VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline_layout,
								0, 1, m_descriptors[global_mesh].descriptor_set, dynamic_offsets.size(), dynamic_offsets.data());
							VkBuffer vertexBuffers[4] = { mesh.mesh_vertex_position_buffer,mesh.mesh_vertex_normal_buffer,mesh.mesh_vertex_tangent_buffer,mesh.mesh_vertex_uv_buffer };
							VkBuffer indices_buffer = mesh.mesh_vertex_index_buffer;
							size_t indices_count = mesh.index_count;
							VkDeviceSize offsets[] = { 0,0,0,0 };

							vkCmdBindVertexBuffers(m_vulkan_rhi->getCurrentCommandBuffer(), 0, (sizeof(vertexBuffers) / sizeof(vertexBuffers[0])), vertexBuffers, offsets);

							vkCmdBindIndexBuffer(m_vulkan_rhi->getCurrentCommandBuffer(), indices_buffer, 0, VK_INDEX_TYPE_UINT16);
							vkCmdDrawIndexed(m_vulkan_rhi->getCurrentCommandBuffer(), static_cast<uint32_t>(indices_count), current_instance_count, 0, 0, 0);
						}
					}
				}
			}

		}
	}

	void MainCameraRenderPass::drawSkyBox()
	{

		VkCommandBuffer command_buffer = m_vulkan_rhi->getCurrentCommandBuffer();
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_skybox].pipeline);
		vkCmdSetViewport(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &m_vulkan_rhi->getViewport());

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_vulkan_rhi->getSwapChainExtent();
		vkCmdSetScissor(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &scissor);

		uint32_t perframe_dynamic_offset = roundUp(m_global_render_resource->storage_buffer.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()], m_global_render_resource->storage_buffer.min_storage_buffer_offset_alignment);
		m_global_render_resource->storage_buffer.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] = perframe_dynamic_offset + sizeof(MeshPerFrameStorageBufferObject);
		assert(m_global_render_resource->storage_buffer.ringbuffer_end[m_vulkan_rhi->getCurrentFrameIndex()] <=
			m_global_render_resource->storage_buffer.ringbuffer_begin[m_vulkan_rhi->getCurrentFrameIndex()] + m_global_render_resource->storage_buffer.ringbuffer_size[m_vulkan_rhi->getCurrentFrameIndex()]);

		(*reinterpret_cast<MeshPerFrameStorageBufferObject*>(reinterpret_cast<uintptr_t>(m_global_render_resource->storage_buffer.global_ringbuffer_memory_pointer) + perframe_dynamic_offset)) = m_mesh_perframe_storage_buffer_object;

		uint32_t dynamic_offset[3] = { perframe_dynamic_offset,0,0 };
		std::array<VkDescriptorSet, 2> descriptor_sets = { *m_descriptors[global_mesh].descriptor_set,*m_descriptors[skybox].descriptor_set };
		vkCmdBindDescriptorSets(m_vulkan_rhi->getCurrentCommandBuffer(), VK_PIPELINE_BIND_POINT_GRAPHICS,
			m_render_pipelines[render_pipeline_type_skybox].pipeline_layout, 0, descriptor_sets.size(),
			descriptor_sets.data(), (sizeof(dynamic_offset) / sizeof(dynamic_offset[0])), dynamic_offset);

		VkDeviceSize offset[1] = { 0 };
		//vkCmdBindVertexBuffers(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, VK_NULL_HANDLE, offset);
		vkCmdDraw(m_vulkan_rhi->getCurrentCommandBuffer(), 36, 1, 0, 0);
	}

	void MainCameraRenderPass::drawMSAA()
	{
		VkCommandBuffer command_buffer = m_vulkan_rhi->getCurrentCommandBuffer();
		vkCmdBindPipeline(command_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_render_pipelines[render_pipeline_type_mesh_global_buffer].pipeline);
		vkCmdSetViewport(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &m_vulkan_rhi->getViewport());

		VkRect2D scissor{};
		scissor.offset = { 0, 0 };
		scissor.extent = m_vulkan_rhi->getSwapChainExtent();
		vkCmdSetScissor(m_vulkan_rhi->getCurrentCommandBuffer(), 0, 1, &scissor);

		//vkCmdDraw(m_vulkan_rhi->getCurrentCommandBuffer(),3,1,0,0);
	}



}


