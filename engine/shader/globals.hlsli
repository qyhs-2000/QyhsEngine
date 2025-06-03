#ifndef QYHS_SHADER_GLOBALS
#define QYHS_SHADER_GLOBALS

#include "ColorSpaceUtility.hlsli"

#if defined(__spirv__)

static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER = 1;
static const uint DESCRIPTOR_SET_BINDLESS_UNIFORM_TEXEL_BUFFER = 2;
static const uint DESCRIPTOR_SET_BINDLESS_SAMPLER = 3;
static const uint DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE = 4;
static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_IMAGE = 5;
static const uint DESCRIPTOR_SET_BINDLESS_STORAGE_TEXEL_BUFFER = 6;
static const uint DESCRIPTOR_SET_BINDLESS_ACCELERATION_STRUCTURE = 7;

[[vk::binding(0,DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER)]] ByteAddressBuffer 
bindless_buffers[];
[[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLER)]] SamplerState bindless_samplers[];
[[vk::binding(0, DESCRIPTOR_SET_BINDLESS_UNIFORM_TEXEL_BUFFER)]] Buffer<float4> bindless_buffers_float4[];
[[vk::binding(0, DESCRIPTOR_SET_BINDLESS_SAMPLED_IMAGE)]] Texture2D bindless_textures[];

#include "shader_interop_renderer.h"

[[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER)]] StructuredBuffer<ShaderGeometry> bindless_structured_geometry[];
[[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER)]] StructuredBuffer<ShaderMeshInstance> bindless_structured_meshinstance[];
[[vk::binding(0, DESCRIPTOR_SET_BINDLESS_STORAGE_BUFFER)]] StructuredBuffer<ShaderMaterial> bindless_structured_material[];

#else   //hlsl

SamplerState bindless_samplers[] : register(space1);
Texture2D bindless_textures[] : register(space2);
ByteAddressBuffer bindless_buffers[] : register(space3);

#endif  //__spirv__

// Creates a full screen triangle from 3 vertices:
inline void vertexID_create_fullscreen_triangle(in uint vertexID, out float4 pos)
{
	pos.x = (float)(vertexID / 2) * 4.0 - 1.0;
	pos.y = (float)(vertexID % 2) * 4.0 - 1.0;
	pos.z = 0;
	pos.w = 1;
}

inline FrameConstantBuffer getFrame()
{
	return g_frame;
}
inline ShaderScene getScene()
{
	return getFrame().scene;
}

inline float2 clipspace_to_uv(in float2 clipspace)
{
	return clipspace * float2(0.5, -0.5) + 0.5;
}
inline float3 clipspace_to_uv(in float3 clipspace)
{
	return clipspace * float3(0.5, -0.5, 0.5) + 0.5;
}
inline half2 clipspace_to_uv(in half2 clipspace)
{
	return clipspace * half2(0.5, -0.5) + 0.5;
}
inline half3 clipspace_to_uv(in half3 clipspace)
{
	return clipspace * half3(0.5, -0.5, 0.5) + 0.5;
}

inline half4 unpack_color(uint2 value)
{
	half4 retVal;
	retVal.x = (half)f16tof32(value.x);
	retVal.y = (half)f16tof32(value.x >> 16u);
	retVal.z = (half)f16tof32(value.y);
	retVal.w = (half)f16tof32(value.y >> 16u);
	return retVal;
}

inline ShaderMeshInstance loadInstance(uint instance_index)
{
	return bindless_structured_meshinstance[getScene().instance_buffer][instance_index];
}

inline ShaderGeometry loadGeometry(uint geometry_index)
{
	return bindless_structured_geometry[getScene().geometry_buffer][geometry_index];
}
inline ShaderCamera getCamera(uint camera_index = 0)
{
	return g_camera.cameras[camera_index];
}

inline ShaderMaterial loadMaterial(uint material_index)
{
	return bindless_structured_material[getScene().material_buffer][material_index];
}

SamplerState sampler_linear_clamp : register(s100);

#endif //QYHS_SHADER_GLOBALS