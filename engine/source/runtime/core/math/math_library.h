#pragma once
#include "function/render/primitive.h"
namespace qyhs::math
{
	XMFLOAT4X4 getIdentityMatrix();
	
	XMFLOAT3 min(const XMFLOAT3& a, const XMFLOAT3& b);

	template<typename T>
	T min(const T& a, const T& b)
	{
		return a < b ? a : b;
	}
	template<typename T>
	T max(const T& a, const T& b)
	{
		return a > b ? a : b;
	}
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

	//-----------------------------------------------------------------------------
	// Compute the intersection of a ray (Origin, Direction) with a triangle
	// (V0, V1, V2).  Return true if there is an intersection and also set *pDist
	// to the distance along the ray to the intersection.
	//
	// The algorithm is based on Moller, Tomas and Trumbore, "Fast, Minimum Storage
	// Ray-Triangle Intersection", Journal of Graphics Tools, vol. 2, no. 1,
	// pp 21-28, 1997.
	//
	//	Modified for WickedEngine to return barycentrics and support TMin, TMax
	//-----------------------------------------------------------------------------
	_Use_decl_annotations_
		inline bool XM_CALLCONV RayTriangleIntersects(
			FXMVECTOR Origin,
			FXMVECTOR Direction,
			FXMVECTOR V0,
			GXMVECTOR V1,
			HXMVECTOR V2,
			float& Dist,
			XMFLOAT2& bary,
			float TMin = 0,
			float TMax = std::numeric_limits<float>::max()
		)
	{
		const XMVECTOR g_RayEpsilon = XMVectorSet(1e-20f, 1e-20f, 1e-20f, 1e-20f);
		const XMVECTOR g_RayNegEpsilon = XMVectorSet(-1e-20f, -1e-20f, -1e-20f, -1e-20f);

		XMVECTOR Zero = XMVectorZero();

		XMVECTOR e1 = XMVectorSubtract(V1, V0);
		XMVECTOR e2 = XMVectorSubtract(V2, V0);

		// p = Direction ^ e2;
		XMVECTOR p = XMVector3Cross(Direction, e2);

		// det = e1 * p;
		XMVECTOR det = XMVector3Dot(e1, p);

		XMVECTOR u, v, t;

		if (XMVector3GreaterOrEqual(det, g_RayEpsilon))
		{
			// Determinate is positive (front side of the triangle).
			XMVECTOR s = XMVectorSubtract(Origin, V0);

			// u = s * p;
			u = XMVector3Dot(s, p);

			XMVECTOR NoIntersection = XMVectorLess(u, Zero);
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(u, det));

			// q = s ^ e1;
			XMVECTOR q = XMVector3Cross(s, e1);

			// v = Direction * q;
			v = XMVector3Dot(Direction, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(v, Zero));
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(XMVectorAdd(u, v), det));

			// t = e2 * q;
			t = XMVector3Dot(e2, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(t, Zero));

			if (XMVector4EqualInt(NoIntersection, XMVectorTrueInt()))
			{
				Dist = 0.f;
				return false;
			}
		}
		else if (XMVector3LessOrEqual(det, g_RayNegEpsilon))
		{
			// Determinate is negative (back side of the triangle).
			XMVECTOR s = XMVectorSubtract(Origin, V0);

			// u = s * p;
			u = XMVector3Dot(s, p);

			XMVECTOR NoIntersection = XMVectorGreater(u, Zero);
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(u, det));

			// q = s ^ e1;
			XMVECTOR q = XMVector3Cross(s, e1);

			// v = Direction * q;
			v = XMVector3Dot(Direction, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(v, Zero));
			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorLess(XMVectorAdd(u, v), det));

			// t = e2 * q;
			t = XMVector3Dot(e2, q);

			NoIntersection = XMVectorOrInt(NoIntersection, XMVectorGreater(t, Zero));

			if (XMVector4EqualInt(NoIntersection, XMVectorTrueInt()))
			{
				Dist = 0.f;
				return false;
			}
		}
		else
		{
			// Parallel ray.
			Dist = 0.f;
			return false;
		}

		t = XMVectorDivide(t, det);

		const XMVECTOR invdet = XMVectorReciprocal(det);
		XMStoreFloat(&bary.x, XMVectorMultiply(u , invdet));
		XMStoreFloat(&bary.y, XMVectorMultiply(v , invdet));

		// Store the x-component to *pDist
		XMStoreFloat(&Dist, t);

		if (Dist > TMax || Dist < TMin)
			return false;

		return true;
	}

	const bool collision2D(const XMFLOAT2& hb_pos1, const XMFLOAT2& hb_size1, const XMFLOAT2& hb_pos2, const XMFLOAT2& hb_size2);
	const float lerp(float value1, float value2, float amount);
}