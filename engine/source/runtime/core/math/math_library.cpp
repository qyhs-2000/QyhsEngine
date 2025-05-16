#include "math_library.h"
#include <cmath>
#include <algorithm>

template <typename T>
constexpr T inverse_lerp(T value1, T value2, T pos)
{
	return value2 == value1 ? T(0) : ((pos - value1) / (value2 - value1));
}

namespace qyhs::math
{
	XMFLOAT3 min(const XMFLOAT3& a, const XMFLOAT3& b)
	{
		return XMFLOAT3(std::min(a.x,b.x),std::min(a.y,b.y),std::min(a.z,b.z));
	}

	XMFLOAT3 max(const XMFLOAT3& a, const XMFLOAT3& b)
	{
		return XMFLOAT3(std::max(a.x,b.x),std::max(a.y,b.y),std::max(a.z,b.z));
	}

	constexpr float inverseLerp(float value1, float value2, float pos)
	{
		return ::inverse_lerp(value1, value2, pos);
	}

	XMFLOAT3 inverseLerp(XMFLOAT3 min, XMFLOAT3 max, XMFLOAT3 pos)
	{
		return XMFLOAT3(inverseLerp(min.x,max.x,pos.x),inverseLerp(min.y,max.y,pos.y),inverseLerp(min.z,max.z,pos.z));
	}
}