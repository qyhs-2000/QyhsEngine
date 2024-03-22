#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>
namespace QYHS
{

	struct Material
	{
		//common base texture image
		VkImage	base_color_texture_image;
		VkImageView	base_color_texture_image_view;
		VkDescriptorSet material_descriptor_set;
	};

	struct Mesh
	{
		VkBuffer mesh_vertex_position_buffer;
		VkBuffer mesh_vertex_index_buffer;
		size_t indices_size;
	};

	struct RenderMeshNode
	{
		glm::mat4 model_matrix;
		Mesh* p_mesh;
		Material* p_material;
	};
}