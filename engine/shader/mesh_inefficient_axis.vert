#version 310 es


#define m_mesh_per_drawcall_max_instance_count 64
#define CHAOS_LAYOUT_MAJOR row_major
layout(CHAOS_LAYOUT_MAJOR) buffer;
layout(CHAOS_LAYOUT_MAJOR) uniform;

layout(set = 0, binding = 0) readonly buffer _unused_name_perframe
{
    mat4 proj_view_matrix;
    
};

layout(set = 0, binding = 1) readonly buffer _unused_name_perdrawcall
{
    mat4 model_matrices[m_mesh_per_drawcall_max_instance_count];
    uint node_ids[m_mesh_per_drawcall_max_instance_count];
    
};


layout(location = 0) in vec3 in_position;

layout(location = 0) flat out highp uint out_nodeid;

void main()
{
    highp mat4 model_matrix = model_matrices[gl_InstanceIndex];

    highp vec3 model_position = in_position;
    
    gl_Position = proj_view_matrix * model_matrix * vec4(in_position, 1.0);

    out_nodeid = node_ids[gl_InstanceIndex];
}