#pragma once
#include "core/math/directx_math.h"
#include <limits>
#include <cmath>
namespace qyhs::primitive
{

	class AABB
	{
	public:
		bool isValid() const;
		XMFLOAT3 _min;
		XMFLOAT3 _max;
		
		AABB(
			const XMFLOAT3& _min = XMFLOAT3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()),
			const XMFLOAT3& _max = XMFLOAT3(std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest(), std::numeric_limits<float>::lowest())
		) : _min(_min), _max(_max) {}

		AABB transform(const XMMATRIX& mat) const
		{
			const XMVECTOR vcorners[8] = {
				XMVector3Transform(XMLoadFloat3(&_min), mat),
				XMVector3Transform(XMVectorSet(_min.x, _max.y, _min.z, 1), mat),
				XMVector3Transform(XMVectorSet(_min.x, _max.y, _max.z, 1), mat),
				XMVector3Transform(XMVectorSet(_min.x, _min.y, _max.z, 1), mat),
				XMVector3Transform(XMVectorSet(_max.x, _min.y, _min.z, 1), mat),
				XMVector3Transform(XMVectorSet(_max.x, _max.y, _min.z, 1), mat),
				XMVector3Transform(XMLoadFloat3(&_max), mat),
				XMVector3Transform(XMVectorSet(_max.x, _min.y, _max.z, 1), mat),
			};
			XMVECTOR vmin = vcorners[0];
			XMVECTOR vmax = vcorners[0];
			vmin = XMVectorMin(vmin, vcorners[1]);
			vmax = XMVectorMax(vmax, vcorners[1]);
			vmin = XMVectorMin(vmin, vcorners[2]);
			vmax = XMVectorMax(vmax, vcorners[2]);
			vmin = XMVectorMin(vmin, vcorners[3]);
			vmax = XMVectorMax(vmax, vcorners[3]);
			vmin = XMVectorMin(vmin, vcorners[4]);
			vmax = XMVectorMax(vmax, vcorners[4]);
			vmin = XMVectorMin(vmin, vcorners[5]);
			vmax = XMVectorMax(vmax, vcorners[5]);
			vmin = XMVectorMin(vmin, vcorners[6]);
			vmax = XMVectorMax(vmax, vcorners[6]);
			vmin = XMVectorMin(vmin, vcorners[7]);
			vmax = XMVectorMax(vmax, vcorners[7]);

			XMFLOAT3 min, max;
			XMStoreFloat3(&min, vmin);
			XMStoreFloat3(&max, vmax);
			return AABB(min, max);
		}

		inline XMMATRIX getUnormRemapMatrix()const
		{
			return XMMatrixScaling(_max.x - _min.x, _max.y - _min.y, _max.z - _min.z) * XMMatrixTranslation(_min.x, _min.y, _min.z);
		}

		XMFLOAT3 getCenter()const
		{
			return XMFLOAT3((_min.x + _max.x) * 0.5f, (_min.y + _max.y) * 0.5f, (_min.z + _max.z) * 0.5f);
		}

		XMFLOAT3 getHalfWidth() const
		{
			XMFLOAT3 center = getCenter();
			return XMFLOAT3(abs(_max.x - center.x), abs(_max.y - center.y), abs(_max.z - center.z));
		}

		float getRadius()const
		{
			XMFLOAT3 abc = getHalfWidth();
			return std::sqrt(std::pow(abc.x, 2.0f) + std::pow(abc.y, 2.0f) + std::pow(abc.z, 2.0f));
		}
	private:
	};

	class Frustum
	{
	public:
		bool checkBox(const AABB& aabb);  //true if aabb can't be culled
		XMFLOAT4 planes[6];
	private:
	};
}