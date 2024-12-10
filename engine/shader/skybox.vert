#version 310 es

layout(location = 0) out vec3 frag_UVW;

#define CHAOS_LAYOUT_MAJOR row_major
layout(CHAOS_LAYOUT_MAJOR) buffer;

layout(set = 0,binding = 1) readonly buffer perframe_buffer
{
    highp mat4 proj_view_matrix;
    highp vec3 camera_position;
};

void main()
{

    //gl_Position  = proj_view_matrix*vec4(position,1.0);
    //frag_UVW = position;
    //frag_UVW *= -1.0;

    const vec3 cube_corner_vertex_offsets[8] = vec3[8](vec3(1.0, 1.0, 1.0),
                                                       vec3(1.0, 1.0, -1.0),
                                                       vec3(1.0, -1.0, -1.0),
                                                       vec3(1.0, -1.0, 1.0),
                                                       vec3(-1.0, 1.0, 1.0),
                                                       vec3(-1.0, 1.0, -1.0),
                                                       vec3(-1.0, -1.0, -1.0),
                                                       vec3(-1.0, -1.0, 1.0));

    const int cube_triangle_index[36] = int[36](0,
                                                1,
                                                2,
                                                2,
                                                3,
                                                0,
                                                4,
                                                5,
                                                1,
                                                1,
                                                0,
                                                4,
                                                7,
                                                6,
                                                5,
                                                5,
                                                4,
                                                7,
                                                3,
                                                2,
                                                6,
                                                6,
                                                7,
                                                3,
                                                4,
                                                0,
                                                3,
                                                3,
                                                7,
                                                4,
                                                1,
                                                5,
                                                6,
                                                6,
                                                2,
                                                1); // x+, y+, x-, y-, z+, z-

    // vec3 world_position = camera_position + (camera_z_far_plane / 1.733) *
    // cube_corner_vertex_offsets[cube_triangle_index[gl_VertexIndex]];
    vec3 world_position = camera_position + cube_corner_vertex_offsets[cube_triangle_index[gl_VertexIndex]];

    // world to NDC
    vec4 clip_position = proj_view_matrix * vec4(world_position, 1.0);

    // depth set to 0.99999?
    clip_position.z = clip_position.w * 0.99999;
    gl_Position     = clip_position;

    frag_UVW = normalize(cube_corner_vertex_offsets[cube_triangle_index[gl_VertexIndex]]);

}