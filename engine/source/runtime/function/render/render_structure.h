#pragma once
#include <vulkan/vulkan.h>
#include "core/math/matrix4.h"
namespace QYHS
{

	struct VulkanMaterial
	{
		//common base texture image
		VkImage	base_color_texture_image;
		VkImageView	base_color_texture_image_view;
		VkDeviceMemory base_color_texture_image_memory;

		VkBuffer material_uniform_buffer;
		VkDeviceMemory material_uniform_buffer_memory;
		void* uniform_buffer_mapped_data;
		VkDescriptorSet material_descriptor_set;
	};

	struct VulkanMesh
	{
		VkBuffer mesh_vertex_position_buffer;
		VkDeviceMemory mesh_vertex_position_buffer_memory;

		VkBuffer mesh_vertex_normal_buffer;
		VkDeviceMemory mesh_vertex_normal_buffer_memory;

		VkBuffer mesh_vertex_tangent_buffer;
		VkDeviceMemory mesh_vertex_tangent_buffer_memory;

		VkBuffer mesh_vertex_uv_buffer;
		VkDeviceMemory mesh_vertex_uv_buffer_memory;
		
		VkBuffer mesh_vertex_index_buffer;
		VkDeviceMemory mesh_vertex_index_buffer_memory;

		size_t indices_count;
	};

	struct RenderMeshNode
	{
		Matrix4x4 model_matrix;
		VulkanMesh* p_mesh;
		VulkanMaterial* p_material;
		uint32_t node_id;
	};
}