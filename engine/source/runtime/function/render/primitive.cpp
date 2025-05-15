#include "primitive.h"

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

}
