#version 310 es
#extension GL_GOOGLE_include_directive : enable

#define m_mesh_per_drawcall_max_instance_count 64
#define m_mesh_vertex_blending_max_joint_count 1024

#define CHAOS_LAYOUT_MAJOR row_major
layout(CHAOS_LAYOUT_MAJOR) buffer;
//layout(CHAOS_LAYOUT_MAJOR) uniform;

struct VulkanMeshInstance
{
	highp float enable_vertex_blending;
    highp mat4  model_matrix;
};

struct VulkanMeshVertexJointBinding
{
	highp ivec4 indices;
    highp vec4  weights;
};

layout(set = 0,binding = 1) readonly buffer _unused_name_per_frame
{
	mat4 proj_view_matrix;
	highp vec3  camera_position;
};

layout(set = 0, binding = 2) readonly buffer _unused_name_per_drawcall
{
    VulkanMeshInstance mesh_instances[m_mesh_per_drawcall_max_instance_count];
};

layout(set = 0,binding = 3) readonly buffer _unused_name_per_drawcall_vertex_blending
{
	highp mat4 joint_matrices[m_mesh_vertex_blending_max_joint_count * m_mesh_per_drawcall_max_instance_count];
};

layout(set = 1, binding = 0) readonly buffer _unused_name_per_mesh_joint_binding
{
    VulkanMeshVertexJointBinding indices_and_weights[];
};

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 in_tangent;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
void main()
{
	highp vec3 model_position;
	highp mat4 model_matrix = mesh_instances[gl_InstanceIndex].model_matrix;
	highp float enable_vertex_blending = mesh_instances[gl_InstanceIndex].enable_vertex_blending;
	if(enable_vertex_blending > 0.0)
	{
		highp ivec4 in_indices = indices_and_weights[gl_VertexIndex].indices;
		highp vec4 in_weights = indices_and_weights[gl_VertexIndex].weights;
		highp mat4 vertex_blending_matrix = mat4x4(
					vec4(0.0,0.0,0.0,0.0),
					vec4(0.0,0.0,0.0,0.0),
					vec4(0.0,0.0,0.0,0.0),
					vec4(0.0,0.0,0.0,0.0)
		); 
		if(in_indices.x>0 && in_weights.x>0.0)
		{
			vertex_blending_matrix += joint_matrices[m_mesh_vertex_blending_max_joint_count * gl_InstanceIndex + in_indices.x]*in_weights.x;
		}
		if(in_indices.y>0 && in_weights.y>0.0)
		{
			vertex_blending_matrix += joint_matrices[m_mesh_vertex_blending_max_joint_count * gl_InstanceIndex + in_indices.y]*in_weights.y;
		}
		if(in_indices.z>0 && in_weights.z>0.0)
		{
			vertex_blending_matrix += joint_matrices[m_mesh_vertex_blending_max_joint_count * gl_InstanceIndex + in_indices.z]*in_weights.z;
		}
		if(in_indices.w>0 && in_weights.w>0.0)
		{
			vertex_blending_matrix += joint_matrices[m_mesh_vertex_blending_max_joint_count * gl_InstanceIndex + in_indices.w]*in_weights.w;
		}
		model_position = (vertex_blending_matrix * vec4(inPosition,1.0)).xyz;

	}
	else
	{
		model_position = inPosition;
	}
	vec3 world_position = (model_matrix*vec4(model_position,1.0)).xyz;
	gl_Position = proj_view_matrix*vec4(world_position,1.0);
	fragTexCoord = inTexCoord;
}