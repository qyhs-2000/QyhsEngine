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
		Vector3 camera_position;
	};

	struct MeshInefficientPickPerFrameStorageBuffer
	{
		Matrix4x4 project_view_matrix;
	};

	struct AxisStorageBufferObject
	{
		Matrix4x4 model_matrix;
		unsigned int selected_axis;
	};
	
	struct MeshInefficientPickPerDrawCallStorageBuffer
	{
		Matrix4x4 model_matrices[mesh_per_drawcall_max_instance_count];
		uint32_t node_ids[mesh_per_drawcall_max_instance_count];
	};
	
}