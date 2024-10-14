#version 310 es

layout(location = 0) in highp vec3 frag_UVW;

layout(location = 0) out highp vec4 out_color;

layout(set = 0,binding = 1) readonly buffer perframe_buffer
{
	highp mat4 proj_view_matrix;
	highp vec3 camera_position;
};

layout(set = 1,binding = 0) uniform samplerCube skybox_cube_map;
layout(input_attachment_index = 0,set = 1,binding = 1) uniform highp subpassInput in_base_color;
layout(input_attachment_index = 1,set = 1,binding = 2) uniform highp subpassInput in_scene_depth;

highp vec2 ndcxy_to_uv(highp vec2 ndcxy) { return ndcxy * vec2(0.5, 0.5) + vec2(0.5, 0.5); }

highp vec2 uv_to_ndcxy(highp vec2 uv) { return uv * vec2(2.0, 2.0) + vec2(-1.0, -1.0); }

void main()
{
	highp vec3 previous_color = subpassLoad(in_base_color).rgb;



	highp float depth = subpassLoad(in_scene_depth).r;
	highp vec3 origin_sample_UVW = vec3(frag_UVW.x, frag_UVW.z, frag_UVW.y);

	if(depth>0.9999f)
	{
		highp vec3 result_color = textureLod(skybox_cube_map,origin_sample_UVW,0.0).rgb;
		out_color = vec4(result_color,1.0);
	}
	else
	{
		out_color = vec4(previous_color,1.0f);
	}
	
	
}