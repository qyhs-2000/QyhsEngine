#version 310 es

#include "constants.h"

layout(location = 0) in vec3 in_position;
layout(location = 1) in vec3 in_normal;
layout(location = 2) in vec3 tangent;
layout(location = 3) in vec2 in_texcoord;

layout(location = 0) out vec3 out_color;

layout(set = 0,binding = 0) readonly buffer perframe_storage_buffer
{
	mat4 proj_view_matrix;
	highp vec3  camera_position;
};

layout(set = 0,binding = 1) readonly buffer axis_storage_buffer
{
	mat4 model_matrix;
	uint selected_axis;
};


void main()
{
	vec3 world_position = (model_matrix * vec4(in_position,1.0f)).xyz;
	vec4 clip_position = proj_view_matrix * vec4(world_position,1.0f);
	clip_position.z *= 0.0001;
	gl_Position = clip_position;
	if(in_texcoord.x < 0.01)
	{
		if(selected_axis == uint(0))
		{
			out_color = vec3(1.0,1.0,0.0);
		}
		else
		{
			out_color = vec3(1.0,0.0,0.0);
		}
	}
	else if(in_texcoord.x < 1.01)
	{
		if(selected_axis == uint(1))
		{
			out_color = vec3(1.0,1.0,0.0);
		}
		else
		{
			out_color = vec3(0.0,1.0,0.0);
		}
		
	}
	else if(in_texcoord.x < 2.01)
	{
		if(selected_axis == uint(2))
		{
			out_color = vec3(1.0,1.0,0.0);
		}
		else
		{
			out_color = vec3(0.0,0.0,1.0);
		}
		
	}

}