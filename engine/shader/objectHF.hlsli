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
};

struct VertexInput
{
	uint vertex_id:SV_VertexID;
	uint instance_id:SV_InstanceID;
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
		//position = mul(input.getInstance().transform.getMatrix(),position);
	}
};

