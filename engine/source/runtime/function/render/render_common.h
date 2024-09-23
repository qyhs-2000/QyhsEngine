#pragma once

namespace QYHS
{
	static uint32_t const mesh_per_drawcall_max_instance_count = 64;
	static uint32_t const s_mesh_vertex_blending_max_joint_count = 1024;
	static uint32_t const s_max_point_light_count = 15;

	struct VulkanMeshInstance
	{
		Matrix4x4 model_matrix;
	};

	struct MeshPerdrawcallStorageBufferObject
	{
		VulkanMeshInstance mesh_instances[mesh_per_drawcall_max_instance_count];
	};

	struct MeshPerFrameStorageBufferObject
	{
		Matrix4x4 project_view_matrix;
	};
}