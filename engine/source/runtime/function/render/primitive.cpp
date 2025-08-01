#include "primitive.h"
#include "core/math/math_library.h"
namespace qyhs::primitive
{
	bool Frustum::checkBox(const AABB& aabb)
	{
		if (!aabb.isValid())
		{
			return false;
		}

		XMVECTOR min = XMLoadFloat3(&aabb._min);
		XMVECTOR max = XMLoadFloat3(&aabb._max);
		XMVECTOR zero = XMVectorZero();
		for (int i = 0; i < 6; ++i)
		{
			XMVECTOR plane = XMLoadFloat4(&planes[i]);
			XMVECTOR lt = XMVectorLess(plane, zero);
			XMVECTOR furthestFromPlane = XMVectorSelect(max, min, lt);
			if (XMVectorGetX(XMPlaneDotCoord(plane, furthestFromPlane)) < 0.0f)
			{
				return false;
			}
		}
		return true;
	}

	bool AABB::isValid() const
	{
		if (_min.x > _max.x || _min.y > _max.y || _min.z > _max.z)
			return false;
		return true;
	}

	AABB AABB::Merge(const AABB& a, const AABB& b)
	{
		return AABB(math::min(a.getMin(), b.getMin()), math::max(a.getMax(), b.getMax()));
	}



	bool AABB::intersects(const Ray& ray) const
	{
		if (!isValid())
		{
			return false;
		}
		if (intersects(ray.origin))
		{
			return true;
		}

		XMFLOAT3 MIN = getMin();
		XMFLOAT3 MAX = getMax();

		float tx1 = (MIN.x - ray.origin.x) * ray.direction_inverse.x;
		float tx2 = (MAX.x - ray.origin.x) * ray.direction_inverse.x;

		float tmin = math::min(tx1, tx2);
		float tmax = math::max(tx1, tx2);
		if (ray.TMax < tmin || ray.TMin > tmax)
			return false;

		float ty1 = (MIN.y - ray.origin.y) * ray.direction_inverse.y;
		float ty2 = (MAX.y - ray.origin.y) * ray.direction_inverse.y;

		tmin = math::max(tmin, math::min(ty1, ty2));
		tmax = math::min(tmax, math::max(ty1, ty2));
		if (ray.TMax < tmin || ray.TMin > tmax)
			return false;

		float tz1 = (MIN.z - ray.origin.z) * ray.direction_inverse.z;
		float tz2 = (MAX.z - ray.origin.z) * ray.direction_inverse.z;

		tmin = math::max(tmin, math::min(tz1, tz2));
		tmax = math::min(tmax, math::max(tz1, tz2));
		if (ray.TMax < tmin || ray.TMin > tmax)
			return false;

		return tmax >= tmin;
	}

	bool AABB::intersects(const XMFLOAT3& point) const
	{
		if (!isValid())
			return false;
		if (point.x > _max.x) return false;
		if (point.x < _min.x) return false;
		if (point.y > _max.y) return false;
		if (point.y < _min.y) return false;
		if (point.z > _max.z) return false;
		if (point.z < _min.z) return false;
		return true;
	}

	bool HitBox2D::intersects(const HitBox2D& hb)
	{
		return qyhs::math::collision2D(pos, size, hb.pos, hb.size);
	}

	bool Ray::intersects(const AABB& aabb)const
	{
		return aabb.intersects(*this);
	}

}
