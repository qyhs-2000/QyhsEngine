#pragma once
#include "function/render/primitive.h"
namespace qyhs::math
{
	XMFLOAT4X4 getIdentityMatrix();
	
	XMFLOAT3 min(const XMFLOAT3& a, const XMFLOAT3& b);
	XMFLOAT3 max(const XMFLOAT3& a, const XMFLOAT3& b);

	XMFLOAT3 inverseLerp(XMFLOAT3 min, XMFLOAT3 max, XMFLOAT3 pos);
	constexpr float inverseLerp(float value1, float value2, float pos);

	inline float Distance(XMVECTOR v1, XMVECTOR v2)
	{
		return XMVectorGetX(XMVector3Length(XMVectorSubtract(v1, v2)));
	}
	inline float Distance(const XMFLOAT2& v1, const XMFLOAT2& v2)
	{
		XMVECTOR vector1 = XMLoadFloat2(&v1);
		XMVECTOR vector2 = XMLoadFloat2(&v2);
		return XMVectorGetX(XMVector2Length(vector2 - vector1));
	}
	inline float Distance(const XMFLOAT3& v1, const XMFLOAT3& v2)
	{
		XMVECTOR vector1 = XMLoadFloat3(&v1);
		XMVECTOR vector2 = XMLoadFloat3(&v2);
		return Distance(vector1, vector2);
	}
}