#ifndef SHADER_INTEROP_H
#define SHADER_INTEROP_H
#ifdef __cplusplus    //c++ 

#include "../source/runtime/core/math/directx_math.h"

using float4x4 = XMFLOAT4X4;
using uint = uint32_t;
using uint2 = XMUINT2;
using float3 = XMFLOAT3;
using float4 = XMFLOAT4;
using float2 = XMFLOAT2;
#include "../source/runtime/core/helper.h"
static const std::string SHADER_SOURCE_DIR = qyhs::helper::getCurrentPath()+ "/" + qyhs::helper::getDirectoryFromPath(qyhs::helper::getRelativePath(qyhs::helper::getCurrentPath(), std::string(__FILE__)));

#define CONSTANTBUFFER(name,type,slot)

#define CB_GETBINDSLOT(name) __CBUFFERBINDSLOT__##name##__
#define CBUFFER(name, slot) static const int CB_GETBINDSLOT(name) = slot; struct alignas(16) name

#else      //shader side
#define PASTE(a,b) a##b
#define alignas(x)
#define CBUFFER(name,slot) cbuffer name:register(PASTE(b,slot))
#define CONSTANTBUFFER(name, type, slot) ConstantBuffer< type > name : register(PASTE(b, slot))

#if defined(__spirv__)
#define PUSHCONSTANT(name,type) [[vk::push_constant]] type name;
#else
#define PUSHCONSTANT(name, type) ConstantBuffer<type> name : register(b999)
#endif

#endif //__cplusplus

#define CBSLOT_IMAGE							0
#define CBSLOT_FONT								0
#define CBSLOT_RENDERER_FRAME					0
#define CBSLOT_RENDERER_CAMERA					1

#define CBSLOT_RENDERER_MISC					3

#endif  //SHADER_INTEROP_H