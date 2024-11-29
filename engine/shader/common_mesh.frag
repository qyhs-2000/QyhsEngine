#version 450

layout(set = 2,binding = 0) uniform sampler2D base_color_texture_sampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 frag_texcoord;

layout(location = 0) out vec4 outColor;

highp vec3 getBaseColor()
{
	highp vec3 base_color = texture(base_color_texture_sampler,frag_texcoord).xyz;
	return base_color;	
}

void main()
{
	outColor = vec4(getBaseColor(),1.0);
}