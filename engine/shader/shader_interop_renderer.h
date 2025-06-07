#pragma once
#include "shader_interop.h"
static const int shader_camera_count = 16;
static const uint SHADERTYPE_BIN_COUNT = 11;

#ifndef __cplusplus
#ifdef TEXTURE_SLOT_NONUNIFORM
#define UniformTextureSlot(x) NonUniformResourceIndex(x)
#else
#define UniformTextureSlot(x) (x)
#endif // TEXTURE_SLOT_NONUNIFORM
#endif

enum SHADERCAMERA_OPTIONS
{
	SHADERCAMERA_OPTION_NONE = 0,
	SHADERCAMERA_OPTION_USE_SHADOW_MASK = 1 << 0,
	SHADERCAMERA_OPTION_ORTHO = 1 << 1,
};

struct alignas(16) ShaderTransform
{
	float4 mat0;
	float4 mat1;
	float4 mat2;
	// float4x4(XMFLOAT4X4) is row-major, hlsl prefer column-major
	void create(float4x4 matrix)
	{
		mat0 = float4(matrix._11, matrix._21, matrix._31, matrix._41);
		mat1 = float4(matrix._12, matrix._22, matrix._32, matrix._42);
		mat2 = float4(matrix._13, matrix._23, matrix._33, matrix._43);
	}
	float4x4 getMatrix()
	{
		return float4x4(
			mat0.x, mat0.y, mat0.z, mat0.w,
			mat1.x, mat1.y, mat1.z, mat1.w,
			mat2.x, mat2.y, mat2.z, mat2.w,
			0, 0, 0, 1
		);
	}
	void init()
	{
		mat0 = float4(1, 0, 0, 0);
		mat1 = float4(0, 1, 0, 0);
		mat2 = float4(0, 0, 1, 0);
	}
};

struct alignas(16) ShaderMeshInstance
{
	ShaderTransform transform;
	void init()
	{
		transform.init();
	}
};

enum TEXTURESLOT
{
	BASECOLORMAP,

	TEXTURESLOT_COUNT
};

struct alignas(16) ShaderTextureSlot
{
	uint uvset_aniso_lodclamp;
	int texture_descriptor;
	inline void init()
	{
		uvset_aniso_lodclamp = 0;
		texture_descriptor = -1;
	}
	inline uint getUVSet()
	{
		return uvset_aniso_lodclamp & 1u;
	}
	inline bool isValid()
	{
		return texture_descriptor >= 0;
	}
#ifndef __cplusplus
	Texture2D getTexture()
	{
		return bindless_textures[UniformTextureSlot(texture_descriptor)];
	}

	float4 Sample(in SamplerState sam, in float4 uvsets)
	{
		Texture2D tex = getTexture();
		float2 uv = getUVSet() == 0 ? uvsets.xy : uvsets.zw;
		return tex.Sample(sam, uv);
	}
#endif
};

struct alignas(16) ShaderMaterial
{
	float4 base_color;
	
	int sampler_descriptor;
	float3 _padding;
	
	ShaderTextureSlot textures[TEXTURESLOT_COUNT];
	void init()
	{
		for (int i = 0; i < TEXTURESLOT_COUNT; ++i)
		{
			textures[i].init();
		}
	}
};


struct alignas(16) ShaderGeometry
{
	int index_buffer;
	int vertex_buffer_position_wind;
	int vb_uvs;
	int vb_nor;

	float2 uv_range_min;
	float2 uv_range_max;
	void init()
	{
		index_buffer = -1;
		vertex_buffer_position_wind = -1;
		vb_uvs = -1;
		vb_nor = -1;
		uv_range_min = float2(0, 0);
		uv_range_max = float2(1, 1);
	}
};

struct alignas(16) ShaderCamera
{
	float4x4 view_projection;
	float4x4 view;
	float4x4 projection;
	float4x4 inverse_view;
	float4x4 inverse_projection;
	float4x4 inverse_view_projection;
	float3 position;
	float3 forward;
	float3 up;
	uint options;
	float z_near;
	float z_far;
	float distant_from_origin;
#ifdef __cplusplus
	void init()
	{
		view_projection = {};
		view = {};
		projection = {};
		inverse_projection = {};
		inverse_view = {};
		position = {};
		distant_from_origin = 0;
	}
#endif
};

struct ShaderMeshInstancePointer
{
	uint data;
	void init()
	{
		data = 0;
	}
	void create(uint32_t instance_index)
	{
		data = 0;
		data |= instance_index & 0xFFFFFF;
	}
	uint getInstanceIndex()
	{
		return data & 0xFFFFFF;
	}
};

CBUFFER(MiscConstantBuffer,CBSLOT_RENDERER_MISC)
{
	float4x4 g_xTransform;
	float4 g_xColor;
};

struct alignas(16) CameraConstantBuffer
{
	ShaderCamera cameras[shader_camera_count];
#ifdef __cplusplus
	void init()
	{
		for (int i = 0; i < shader_camera_count; ++i)
		{
			cameras[i].init();
		}
	}
#endif
};

struct ObjectPushConstant
{
	uint32_t geometry_index;
	uint32_t material_index;
	uint32_t instances;			//descriptor index of instance buffer
	uint32_t instance_offset;
};


struct alignas(16) ShaderScene
{
	int geometry_buffer;
	int instance_buffer;
	int material_buffer;
};

//constant buffer in dx = uniform buffer or uniform buffer dynamic in vulkan
struct alignas(16) FrameConstantBuffer
{
	ShaderScene scene;
};
CONSTANTBUFFER(g_frame, FrameConstantBuffer, CBSLOT_RENDERER_FRAME);
CONSTANTBUFFER(g_camera, CameraConstantBuffer, CBSLOT_RENDERER_CAMERA);