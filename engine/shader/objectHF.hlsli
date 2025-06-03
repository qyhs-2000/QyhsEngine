#include "globals.hlsli"
#include "shader_interop_renderer.h"
PUSHCONSTANT(push,ObjectPushConstant)



inline ShaderGeometry getMesh()
{
	return loadGeometry(push.geometry_index);
}

struct PixelInput
{
	precise float4 position:SV_Position;
	float4 uvsets:UVSETS;
	float4 getUVSets()
	{
		return uvsets;
	}

};

struct VertexInput
{
	uint vertex_id:SV_VertexID;
	uint instance_id:SV_InstanceID;

	float4 getUVSets()
	{
		[branch]
		if(getMesh().vb_uvs < 0)
			return 0;
		return lerp(getMesh().uv_range_min.xyxy, getMesh().uv_range_max.xyxy, bindless_buffers_float4[getMesh().vb_uvs][vertex_id]);
	}
	float4 getPosition()
	{
		return bindless_buffers_float4[getMesh().vertex_buffer_position_wind][vertex_id];
	}
	ShaderMeshInstancePointer getInstancePointer()
	{
		if(push.instances>=0)
		{
			return bindless_buffers[push.instances].Load<ShaderMeshInstancePointer>(push.instance_offset + instance_id * sizeof(ShaderMeshInstancePointer));
		}
		ShaderMeshInstancePointer poi;
		poi.init();
		return poi;
	}
	ShaderMeshInstance getInstance()
	{
		if(push.instances>=0)
		{
			return loadInstance(getInstancePointer().getInstanceIndex());
		}
		ShaderMeshInstance mesh_instance;
		mesh_instance.init();
		return mesh_instance;
	}

};

struct VertexSurface
{
	float4 position;
	float4 uvsets;
	half4 color;
	inline void create(in VertexInput input)
	{
		float4 pos = input.getPosition();
		position = float4(pos.xyz,1);
		uvsets = input.getUVSets();
		//float4x4 trans = float4x4(
		//10,0,0,-5,
		//0,0,-10,5,
		//0,10,0,-5,
		//0,0,0,1);

		position = mul(input.getInstance().transform.getMatrix(),position);
		//position = mul(trans,position);
	}
};

inline ShaderMaterial getMaterial()
{
	return loadMaterial(push.material_index);
}

#define sampler_objectshader bindless_samplers[getMaterial().sampler_descriptor]


#ifdef OBJECTSHADER_COMPILER_VS
PixelInput vertex_to_pixel_export(VertexInput input)
{
	PixelInput output;
	VertexSurface surface;
	surface.create(input);
	output.position = surface.position;
	output.uvsets = surface.uvsets;
	ShaderCamera camera = getCamera();
//#ifndef OBJECT_SHADER_NO_USE_CAMERA
	output.position = mul(output.position,camera.view_projection);
//#endif
	return output;
}

//vertex shader base
PixelInput main(VertexInput input)
{
	return vertex_to_pixel_export(input);
}

#endif  //OBJECTSHADER_COMPILER_VS




#ifdef OBJECTSHADER_COMPILER_PS
//Pixel shader entry point

float4 main(PixelInput input) : SV_Target
{
	float4 uvsets = input.getUVSets();
	ShaderMaterial material = getMaterial();
	float4 base_color = material.base_color;
	[branch]
	if(material.textures[BASECOLORMAP].isValid())
	{
		base_color *=(half4)material.textures[BASECOLORMAP].Sample(sampler_objectshader,uvsets);
	}
	half4 color = base_color;

#ifdef PREPASS 
#ifndef DEPTH_ONLY
	return float4(0,0,0,0);
#endif
#else
	return color;
#endif //PREPASS
}

#endif //OBJECTSHADER_COMPILER_PS

