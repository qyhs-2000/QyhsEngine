#include "globals.hlsli"

PUSHCONSTANT(push, SkinningPushConstants);

#ifndef __PSSL__
#undef WICKED_ENGINE_DEFAULT_ROOTSIGNATURE // don't use auto root signature!

#endif // __PSSL__

[numthreads(64, 1, 1)]
void main(uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID)
{
	const uint vertexID = DTid.x;
	
	[branch]
	if (vertexID >= push.vertexCount)
		return;
	
	float4 pos_wind = 0;
	[branch]
	if (push.vb_pos_wind >= 0)
	{
		pos_wind = bindless_buffers_float4[push.vb_pos_wind][vertexID];
	}
	
	
	
	
	
	float3 pos = pos_wind.xyz;
	if(any(push.aabb_min) || any(push.aabb_max))
	{
		// UNORM vertex position remap:
		pos = lerp(push.aabb_min, push.aabb_max, pos);
	}

	ByteAddressBuffer skinningbuffer = bindless_buffers[push.skinningbuffer_index];


	// Skinning:
	[branch]
	if (push.vb_bon >= 0 && push.bone_offset != ~0u)
	{
		ByteAddressBuffer boneBuffer = bindless_buffers[push.vb_bon];
		float4 p = 0;

		half weisum = 0;
		for (uint influence = 0; influence < push.influence_div4; ++influence)
		{
			uint4 ind = 0;
			half4 wei = 0;
			[branch]
			if (push.vb_bon >= 0)
			{
				// Manual type-conversion for bone props:
				uint4 ind_wei_u = boneBuffer.Load4((vertexID * push.influence_div4 + influence) * sizeof(uint4));

				ind.x = uint(ind_wei_u.x & 0xFFFFF);
				ind.y = uint(ind_wei_u.y & 0xFFFFF);
				ind.z = uint(ind_wei_u.z & 0xFFFFF);
				ind.w = uint(ind_wei_u.w & 0xFFFFF);

				wei.x = half(float((ind_wei_u.x >> 20) & 0xFFF) / 4095.0);
				wei.y = half(float((ind_wei_u.y >> 20) & 0xFFF) / 4095.0);
				wei.z = half(float((ind_wei_u.z >> 20) & 0xFFF) / 4095.0);
				wei.w = half(float((ind_wei_u.w >> 20) & 0xFFF) / 4095.0);
			}
			if (any(wei))
			{
				for (uint i = 0; ((i < 4) && (weisum < 1.0)); ++i)
				{
					float4x4 m = skinningbuffer.Load<ShaderTransform>(push.bone_offset + ind[i] * sizeof(ShaderTransform)).getMatrix();
					half weight = wei[i];

					p += mul(m, float4(pos.xyz, 1)) * weight;
					weisum += weight;
				}
			}
		}
		pos.xyz = p.xyz;
	}

	// Store data:
	[branch]
	if (push.so_pos >= 0)
	{
#ifdef __PSSL__
		bindless_rwbuffers[push.so_pos].TypedStore<float3>(vertexID * sizeof(float3), pos);
#else
		bindless_rwbuffers[push.so_pos].Store<float3>(vertexID * sizeof(float3), pos);
#endif // __PSSL__
	}

	
}
