#version 310 es

layout(location = 0) in highp vec3 in_color;
layout(location = 0) out highp vec4 out_color;

layout(input_attachment_index = 0,set = 0,binding = 2) uniform highp subpassInput in_backup_color;
void main()
{
	out_color = vec4(in_color ,1.0);
}