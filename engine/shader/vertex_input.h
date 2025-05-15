#pragma once
// Vertex shader input结构体
struct VSInput
{
    float3 position : POSITION; // 顶点位置
};

// Vertex shader输出结构体
struct VSOutput
{
    float4 position : SV_POSITION; // 裁剪空间位置
};