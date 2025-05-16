// 无需输入 layout，直接使用系统语义 SV_VertexID
struct VSOutput {
    float4 position : SV_POSITION;
    float4 color    : COLOR;
};

VSOutput main(uint vertexID : SV_VertexID) {
    float2 positions[3] = {
        float2( 0.0f,  0.5f),  // Top
        float2( 0.5f, -0.5f),  // Right
        float2(-0.5f, -0.5f)   // Left
    };

    float4 colors[3] = {
        float4(1, 0, 0, 1), // Red
        float4(0, 1, 0, 1), // Green
        float4(0, 0, 1, 1)  // Blue
    };

    VSOutput output;
    output.position = float4(positions[vertexID], 0.0f, 1.0f);
    output.color = colors[vertexID];
    return output;
}
