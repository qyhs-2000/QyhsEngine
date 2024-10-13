#version 310 es

layout(location = 0) in highp vec2 in_texcoord;

layout(location = 0) out highp vec4 out_color;

layout(set = 0,binding = 1) readonly buffer perframe_buffer
{
	highp mat4 proj_view_matrix;
	highp vec3 camera_position;
};

layout(set = 1,binding = 0) uniform samplerCube skybox;
layout(input_attachment_index = 0,set = 1,binding = 1) uniform highp subpassInput in_base_color;
layout(input_attachment_index = 1,set = 1,binding = 2) uniform highp subpassInput in_scene_depth;

highp vec2 ndcxy_to_uv(highp vec2 ndcxy) { return ndcxy * vec2(0.5, 0.5) + vec2(0.5, 0.5); }

highp vec2 uv_to_ndcxy(highp vec2 uv) { return uv * vec2(2.0, 2.0) + vec2(-1.0, -1.0); }

void main()
{
	highp vec3 in_world_position;
    {
        highp float scene_depth              = subpassLoad(in_scene_depth).r;
        highp vec4  ndc                      = vec4(uv_to_ndcxy(in_texcoord), scene_depth, 1.0);
        highp mat4  inverse_proj_view_matrix = inverse(proj_view_matrix);
        highp vec4  in_world_position_with_w = inverse_proj_view_matrix * ndc;
        in_world_position                    = in_world_position_with_w.xyz / in_world_position_with_w.www;
    }

	highp vec4 previous_pass_color = subpassLoad(in_base_color);
	
	highp float depth = subpassLoad(in_scene_depth).r;
	if(depth>=1.0)
	{
		// skybox
        highp vec3 in_UVW            = normalize(in_world_position - camera_position);
        highp vec3 origin_sample_UVW = vec3(in_UVW.x, in_UVW.z, in_UVW.y);

		highp vec3 result_color = textureLod(skybox, origin_sample_UVW, 0.0).rgb;;
		out_color = vec4(result_color,1.0f);
	}
	else
	{
		out_color = previous_pass_color;
	}
	
}