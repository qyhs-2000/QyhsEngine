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
	XMFLOAT4X4 IDENTITY_MATRIX = XMFLOAT4X4(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1);
	XMFLOAT4X4 getIdentityMatrix()
	{
		return IDENTITY_MATRIX;
	}
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

	const bool collision2D(const XMFLOAT2& hitBox1Pos, const XMFLOAT2& hitBox1Siz, const XMFLOAT2& hitBox2Pos, const XMFLOAT2& hitBox2Siz)
	{
		if (hitBox1Siz.x <= 0 || hitBox1Siz.y <= 0 || hitBox2Siz.x <= 0 || hitBox2Siz.y <= 0)
			return false;

		if (hitBox1Pos.x + hitBox1Siz.x < hitBox2Pos.x)
			return false;
		else if (hitBox1Pos.x > hitBox2Pos.x + hitBox2Siz.x)
			return false;
		else if (hitBox1Pos.y + hitBox1Siz.y < hitBox2Pos.y)
			return false;
		else if (hitBox1Pos.y > hitBox2Pos.y + hitBox2Siz.y)
			return false;

		return true;
	}

	XMFLOAT3 inverseLerp(XMFLOAT3 min, XMFLOAT3 max, XMFLOAT3 pos)
	{
		return XMFLOAT3(inverseLerp(min.x,max.x,pos.x),inverseLerp(min.y,max.y,pos.y),inverseLerp(min.z,max.z,pos.z));
	}
}