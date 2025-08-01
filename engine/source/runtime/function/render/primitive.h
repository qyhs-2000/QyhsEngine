#pragma once
#include "core/math/directx_math.h"
#include <limits>
#include <cmath>

namespace qyhs::primitive
{
	class Ray;
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

		static AABB Merge(const AABB& a, const AABB& b);

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

		inline void createFromHalfWidth(const XMFLOAT3& center, const XMFLOAT3& halfwidth)
		{
			_min = XMFLOAT3(center.x - halfwidth.x, center.y - halfwidth.y, center.z - halfwidth.z);
			_max = XMFLOAT3(center.x + halfwidth.x, center.y + halfwidth.y, center.z + halfwidth.z);
		}

		bool intersects(const Ray& ray) const;
		bool intersects(const XMFLOAT3& point) const;

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
		constexpr XMFLOAT3 getMin() const { return _min; }
		constexpr XMFLOAT3 getMax() const { return _max; }
	private:
	};

	class Ray
	{
	public:
		XMFLOAT3 direction;
		XMFLOAT3 origin{ 0,0,0 };
		XMFLOAT3 direction_inverse;
		Ray(const XMFLOAT3& newOrigin = XMFLOAT3(0, 0, 0), const XMFLOAT3& newDirection = XMFLOAT3(0, 0, 1), float newTMin = 0, float newTMax = std::numeric_limits<float>::max()) :
			Ray(XMLoadFloat3(&newOrigin), XMLoadFloat3(&newDirection), newTMin, newTMax)
		{}
		Ray(const XMVECTOR& newOrigin, const XMVECTOR& newDirection, float newTMin = 0, float newTMax = std::numeric_limits<float>::max())
		{
			XMStoreFloat3(&origin, newOrigin);
			XMStoreFloat3(&direction, newDirection);
			XMStoreFloat3(&direction_inverse, XMVectorReciprocal(newDirection));
			TMin = newTMin;
			TMax = newTMax;
		}
		float TMax = std::numeric_limits<float>::max();
		float TMin = 0;
		bool intersects(const AABB& aabb)const;
	private:
	};

	class Frustum
	{
	public:
		bool checkBox(const AABB& aabb);  //true if aabb can't be culled
		XMFLOAT4 planes[6];
	private:
	};

	class HitBox2D
	{
	public:
		HitBox2D() :pos(XMFLOAT2(0, 0)), size(XMFLOAT2(0, 0)) {};
		HitBox2D(XMFLOAT2 new_pos, XMFLOAT2 new_size) :pos(new_pos), size(new_size) {}
		bool intersects(const HitBox2D& hb);
		
		XMFLOAT2 pos;
		XMFLOAT2 size;
	private:
	};

	struct Sphere
	{
		XMFLOAT3 center;
		float radius;
	};

	struct Capsule
	{
		XMFLOAT3 base = XMFLOAT3(0, 0, 0);
		XMFLOAT3 tip = XMFLOAT3(0, 0, 0);
		float radius = 0;
		Capsule() = default;
		Capsule(const XMFLOAT3& base, const XMFLOAT3& tip, float radius) :base(base), tip(tip), radius(radius)
		{
			assert(radius >= 0);
		}
		Capsule(XMVECTOR base, XMVECTOR tip, float radius) :radius(radius)
		{
			assert(radius >= 0);
			XMStoreFloat3(&this->base, base);
			XMStoreFloat3(&this->tip, tip);
		}
		Capsule(const Sphere& sphere, float height) :
			base(XMFLOAT3(sphere.center.x, sphere.center.y - sphere.radius, sphere.center.z)),
			tip(XMFLOAT3(base.x, base.y + height, base.z)),
			radius(sphere.radius)
		{
			assert(radius >= 0);
		}
		inline AABB getAABB() const
		{
			XMFLOAT3 halfWidth = XMFLOAT3(radius, radius, radius);
			AABB base_aabb;
			base_aabb.createFromHalfWidth(base, halfWidth);
			AABB tip_aabb;
			tip_aabb.createFromHalfWidth(tip, halfWidth);
			AABB result = AABB::Merge(base_aabb, tip_aabb);
			assert(result.isValid());
			return result;
		}
		
	};
}