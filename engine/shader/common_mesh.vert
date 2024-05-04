#version 450
layout(set = 0,binding = 0) uniform UniformBufferObject
{
	mat4 model;
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
//layout(location = 1) in vec3 inColor;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;
void main()
{
	highp vec3 model_position;
	model_position = inPosition;

	gl_Position = ubo.proj*ubo.view*vec4(model_position,1.0);
	fragTexCoord = inTexCoord;
}