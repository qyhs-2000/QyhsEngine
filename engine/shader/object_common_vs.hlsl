#include "objectHF.hlsli"

PixelInput vertex_to_pixel_export(VertexInput input)
{
	PixelInput output;
	VertexSurface surface;
	surface.create(input);
	output.position = surface.position;

	ShaderCamera camera = getCamera();
//#ifndef OBJECT_SHADER_NO_USE_CAMERA
	output.position = mul(camera.view_projection,output.position);
//#endif
	return output;
}

//vertex shader base
PixelInput main(VertexInput input)
{
	return vertex_to_pixel_export(input);
}

//PixelInput main(uint vertexID : SV_VertexID) {
//    float2 positions[3] = {
//        float2( 0.0f,  0.5f),  // Top
//        float2( 0.5f, -0.5f),  // Right
//        float2(-0.5f, -0.5f)   // Left
//    };
//
//    float4 colors[3] = {
//        float4(1, 0, 0, 1), // Red
//        float4(0, 1, 0, 1), // Green
//        float4(0, 0, 1, 1)  // Blue
//    };
//
//    PixelInput output;
//    output.position = float4(positions[vertexID], 0.0f, 1.0f);
//    //output.color = colors[vertexID];
//    return output;
//}