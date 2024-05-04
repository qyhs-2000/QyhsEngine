#pragma once
#include "core/meta/reflection/reflection.h"
//#include "matrix3.h"

namespace QYHS
{
	class Matrix3x3;
	class Matrix4x4;
	REFLECTION_TYPE(Quaternion)
		CLASS(Quaternion, Fields)
	{
	public:
		Quaternion() = default;
		Quaternion(float w_, float x_, float y_, float z_) : w{ w_ }, x{ x_ }, y{ y_ }, z{ z_ } {}


		void toRotationMatrix(Matrix4x4& kRot)const;
		void toRotationMatrix(Matrix3x3& rotation) const;
		static const Quaternion ZERO;
		static const Quaternion IDENTITY;
		float w{ 1.f }, x{ 0.f }, y{ 0.f }, z{ 0.f };
	private:
	};
}