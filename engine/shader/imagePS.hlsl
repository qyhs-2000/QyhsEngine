#include "imageHF.hlsli"


float4 main(VertextoPixel input) : SV_TARGET
{
    half4 color = unpack_color(image.packed_color);
    [branch]
    if(image.texture_base_index>=0)
    {
        SamplerState sam = bindless_samplers[image.sampler_index];
        float4 uvsets = input.compute_uvs();
        half4 tex = bindless_textures[image.texture_base_index].Sample(sam,uvsets.xy);
        color *= tex;
    }
    return color; 
}