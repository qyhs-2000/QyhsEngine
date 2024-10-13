#version 310 es
#extension GL_GOOGLE_include_directive : enable

#define m_mesh_per_drawcall_max_instance_count 64
#define m_mesh_vertex_blending_max_joint_count 1024

#define CHAOS_LAYOUT_MAJOR row_major
layout(CHAOS_LAYOUT_MAJOR) buffer;
//layout(CHAOS_LAYOUT_MAJOR) uniform;

struct VulkanMeshInstance
{
    highp mat4  model_matrix;
};


layout(set = 0,binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(set = 0,binding = 1) readonly buffer _unused_name_per_frame
{
	mat4 proj_view_matrix;
	highp vec3  camera_position;
};

layout(set = 0, binding = 2) readonly buffer _unused_name_per_drawcall
{
    VulkanMeshInstance mesh_instances[m_mesh_per_drawcall_max_instance_count];
};

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
void main()
{
	highp vec3 model_position;
	highp mat4 model_matrix = mesh_instances[gl_InstanceIndex].model_matrix;
	model_position = inPosition;
	vec3 world_position = (model_matrix*vec4(model_position,1.0)).xyz;
	//gl_Position = ubo.proj*ubo.view*vec4(inPosition,1.0);
	gl_Position = proj_view_matrix*vec4(world_position,1.0);
	fragTexCoord = inTexCoord;
}